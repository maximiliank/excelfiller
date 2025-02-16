#pragma once

#include "SheetData.h"
#include <pugixml.hpp>
#include <string>

namespace ExcelFiller {
    class XlsxWorkbook;

    class XlsxWorksheet {
        pugi::xml_document doc_;
        XlsxWorkbook& workbook_;
        std::string xmlName_;

      public:
        explicit XlsxWorksheet(pugi::xml_document&& doc, XlsxWorkbook& workbook, std::string xmlName);

        SheetData getSheetData();

        void save();
    };
} // namespace ExcelFiller
