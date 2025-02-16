#include "ExcelFiller/zipxmlhelper.h"
#include <fmt/format.h>
#include <memory>
#include <zip/zip.h>

ExcelFiller::ZipXMLHelper::ZipXMLHelper(const std::string& filename)
    : mode_('r'), zip_(ZipXMLHelper::openArchive(filename.c_str(), 0, mode_)), filename_(filename)
{}

ExcelFiller::ZipXMLHelper::~ZipXMLHelper()
{
    close();
}
bool ExcelFiller::ZipXMLHelper::hasFile(const std::string& file) const
{
    if (zip_entry_open(zip_, file.c_str()) == 0)
    {
        zip_entry_close(zip_);
        return true;
    }
    else
    {
        return false;
    }
}
pugi::xml_document ExcelFiller::ZipXMLHelper::loadXMLFile(const std::string& file)
{
    if (zip_entry_open(zip_, file.c_str()) == 0)
    {
        const auto bufsize = zip_entry_size(zip_);
        auto* buffer = static_cast<char*>(pugi::get_memory_allocation_function()(bufsize));
        zip_entry_noallocread(zip_, buffer, bufsize);

        zip_entry_close(zip_);

        pugi::xml_document doc;
        auto result = doc.load_buffer_inplace_own(buffer, bufsize,
                pugi::parse_minimal | pugi::parse_declaration | pugi::parse_ws_pcdata_single, pugi::encoding_utf8);
        if (!result)
        {
            throw std::runtime_error(fmt::format("XML error {} (offset {})", result.description(), result.offset));
        }
        return doc;
    }
    else
    {
        throw std::runtime_error(fmt::format("Could not open entry {} in zip archive", file));
    }
}

namespace ExcelFiller {
    namespace {
        struct xml_memory_writer : pugi::xml_writer {
            char* buffer_;
            size_t capacity_;

            size_t result_;

            xml_memory_writer() : buffer_(nullptr), capacity_(0), result_(0) {}

            xml_memory_writer(char* buffer, size_t capacity) : buffer_(buffer), capacity_(capacity), result_(0) {}

            [[nodiscard]] size_t written_size() const
            {
                return result_ < capacity_ ? result_ : capacity_;
            }

            void write(const void* data, size_t size) override
            {
                if (result_ < capacity_)
                {
                    size_t chunk = (capacity_ - result_ < size) ? capacity_ - result_ : size;

                    memcpy(buffer_ + result_, data, chunk);
                }

                result_ += size;
            }
        };
    } // namespace
} // namespace ExcelFiller
void ExcelFiller::ZipXMLHelper::writeXMLFile(const std::string& file, const pugi::xml_document& doc)
{
    const auto format = pugi::format_raw | pugi::format_save_file_text | pugi::format_no_escapes;
    const auto encoding = pugi::encoding_utf8;

    reopenFile(0, 'd');
    {
        std::string entry{file};
        char* entries[] = {entry.data()};
        zip_entries_delete(zip_, entries, 1);
    }
    reopenFile(ZIP_DEFAULT_COMPRESSION_LEVEL - 1, 'a');

    // first pass: get required memory size
    xml_memory_writer counter;
    doc.save(counter, "", format, encoding);

    // allocate necessary size (+1 for null termination)
    auto buffer = std::make_unique<char[]>(counter.result_ + 1);

    // second pass: actual printing
    xml_memory_writer writer(buffer.get(), counter.result_);
    doc.save(writer, "", format, encoding);

    // null terminate
    buffer[writer.written_size()] = 0;

    zip_entry_open(zip_, file.c_str());
    {
        zip_entry_write(zip_, buffer.get(), writer.written_size());
    }
    zip_entry_close(zip_);

    reopenFile(0, 'r');
}

void ExcelFiller::ZipXMLHelper::reopenFile(int level, char mode)
{
    if (mode != mode_)
    {
        close();
        zip_ = ZipXMLHelper::openArchive(filename_.c_str(), level, mode);
        mode_ = mode;
    }
}
void ExcelFiller::ZipXMLHelper::close()
{
    if (zip_ != nullptr)
    {
        zip_close(zip_);
    }
}
zip_t* ExcelFiller::ZipXMLHelper::openArchive(const char* zipfile, int level, char mode)
{
    if (auto ptr = zip_open(zipfile, level, mode); ptr != nullptr)
    {
        return ptr;
    }
    else
    {
        throw std::runtime_error(fmt::format("Could not open zip file {}", zipfile));
    }
}
