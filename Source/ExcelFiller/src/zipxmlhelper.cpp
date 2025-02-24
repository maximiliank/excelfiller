#include "ExcelFiller/zipxmlhelper.h"
#include <ZipCpp/zipcpp_flags.hpp>
#include <ZipCpp/zipcompression.h>
#include <cstring>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

ExcelFiller::ZipXMLHelper::ZipXMLHelper(const std::string& filename)
    : archive_(ZipCpp::ZipArchive::open(filename, ZipCpp::LibZipOpen::RDONLY)), filename_(filename)
{}

bool ExcelFiller::ZipXMLHelper::hasFile(const std::string& file) const
{
    return archive_.hasFile(file);
}

pugi::xml_document ExcelFiller::ZipXMLHelper::loadXMLFile(const std::string& file)
{
    std::size_t bufferSize{0};
    auto* buffer = archive_.readFile(file, [&bufferSize](const std::size_t size) {
        bufferSize = size;
        return pugi::get_memory_allocation_function()(size);
    });

    pugi::xml_document doc;
    auto result = doc.load_buffer_inplace_own(buffer, bufferSize,
            pugi::parse_minimal | pugi::parse_declaration | pugi::parse_ws_pcdata_single, pugi::encoding_utf8);
    if (!result)
    {
        throw std::runtime_error(fmt::format("XML error {} (offset {})", result.description(), result.offset));
    }
    return doc;
}

namespace ExcelFiller {
    namespace {
        struct xml_memory_writer : pugi::xml_writer {
            std::byte* buffer_;
            size_t capacity_;

            size_t result_;

            xml_memory_writer() : buffer_(nullptr), capacity_(0), result_(0) {}

            xml_memory_writer(std::byte* buffer, size_t capacity) : buffer_(buffer), capacity_(capacity), result_(0) {}

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
    static const ZipCpp::ZipCompression compression(ZipCpp::ZipCompression::Algorithm::Deflate, std::uint32_t{5});
    const auto format = pugi::format_raw | pugi::format_save_file_text | pugi::format_no_escapes;
    const auto encoding = pugi::encoding_utf8;

    reopenFile(ZipCpp::LibZipOpen::CREATE);

    // first pass: get required memory size
    xml_memory_writer counter;
    doc.save(counter, "", format, encoding);

    // allocate necessary size (null termination not needed for std::byte)
    ZipCpp::MemoryBuffer buffer(counter.result_);

    // second pass: actual printing
    xml_memory_writer writer(buffer.getData().data(), counter.result_);
    doc.save(writer, "", format, encoding);

    archive_.add(file, std::move(buffer), compression);
}
void ExcelFiller::ZipXMLHelper::saveArchive()
{
    // Reopen in read only mode to trigger zip_close in case there was a modification
    reopenFile(ZipCpp::LibZipOpen::RDONLY);
}
void ExcelFiller::ZipXMLHelper::reopenFile(ZipCpp::LibZipOpen flags)
{
    if (archive_.getCurrentFlags() != flags)
    {
        spdlog::debug("Reopening archive with flags {}", flags);
        archive_.writeAndClose();
        archive_ = ZipCpp::ZipArchive::open(filename_, flags);
    }
}
