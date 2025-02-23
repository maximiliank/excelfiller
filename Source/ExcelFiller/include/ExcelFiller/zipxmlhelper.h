#pragma once
#include <pugixml.hpp>
#include <string>
#include <ZipCpp/zipcpp.h>

namespace ExcelFiller {
    class ZipXMLHelper {
        ZipCpp::ZipArchive archive_;
        std::string filename_;

        void reopenFile(ZipCpp::LibZipOpen flags);

      public:
        explicit ZipXMLHelper(const std::string& filename);

      protected:
        [[nodiscard]] bool hasFile(const std::string& file) const;
        [[nodiscard]] pugi::xml_document loadXMLFile(const std::string& file);

        void writeXMLFile(const std::string& file, const pugi::xml_document& doc);

        void saveArchive();
    };
} // namespace ExcelFiller
