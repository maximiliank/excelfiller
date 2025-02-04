#pragma once

#include "ExcelFiller/hashmap.hpp"
#include <pugixml.hpp>
#include <string>
#include <string_view>

namespace ExcelFiller {
    class XlsxWorkbook;
    class SharedStringTable {
        friend class Workbook;

        pugi::xml_document sharedStringTable_;
        XlsxWorkbook& workbook_;
        HashMap<std::string, std::size_t> existingSharedStrings_;
        HashMap<std::string, std::size_t> newlyAddedStrings_;

        std::size_t stringIndexCount_{0};

    public:
        explicit SharedStringTable(pugi::xml_document&& sharedStringTable, XlsxWorkbook& workbook);

        [[nodiscard]] std::size_t getSharedStringIndex(const std::string& item);

        void writeSharedStringTable();
    };
}// namespace ExcelFiller
