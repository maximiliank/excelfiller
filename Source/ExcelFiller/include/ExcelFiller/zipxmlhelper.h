#pragma once
#include <pugixml.hpp>
#include <string>

struct zip_t;
namespace ExcelFiller {
    class ZipXMLHelper {
        char mode_;
        struct zip_t* zip_ = nullptr;
        std::string filename_;

        static zip_t* openArchive(const char* zipname, int level, char mode);
        void reopenFile(int level, char mode);
        void close();

      public:
        explicit ZipXMLHelper(const std::string& filename);

        ~ZipXMLHelper();

      protected:
        [[nodiscard]] bool hasFile(const std::string& file) const;
        [[nodiscard]] pugi::xml_document loadXMLFile(const std::string& file);

        void writeXMLFile(const std::string& file, const pugi::xml_document& doc);
    };
} // namespace ExcelFiller
