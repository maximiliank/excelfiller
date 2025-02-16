#pragma once

#include "ExcelFiller/hashmap.hpp"
#include "ExcelFiller/utils.h"
#include <pugixml.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace ExcelFiller {
    class XlsxWorkbook;
    class SharedStringTable {
        using HashMap_t = HashMap<std::string, std::size_t, ::ExcelFiller::Utils::string_hash, std::equal_to<>>;
        friend class Workbook;

        pugi::xml_document sharedStringTable_;
        XlsxWorkbook& workbook_;
        HashMap_t existingSharedStrings_;
        HashMap_t newlyAddedStrings_;
        std::vector<const char*> newlyAddedStringsOrder_;

        std::size_t stringIndexCount_{0};

      public:
        explicit SharedStringTable(pugi::xml_document&& sharedStringTable, XlsxWorkbook& workbook);
        explicit SharedStringTable(XlsxWorkbook& workbook);

        [[nodiscard]] std::size_t getSharedStringIndex(std::string_view item);

        void writeSharedStringTable();
    };
} // namespace ExcelFiller
