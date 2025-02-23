#pragma once

#include "ExcelFiller/SharedStringTable.h"
#include "workbook.h"
#include "xlsxWorksheet.h"
#include "zipxmlhelper.h"
#include <string>
namespace ExcelFiller {
    class XlsxWorkbook : ZipXMLHelper {
        Workbook workbook_;

      public:
        using ZipXMLHelper::writeXMLFile;
        using ZipXMLHelper::saveArchive;

        explicit XlsxWorkbook(const std::string& filename);

        [[nodiscard]] XlsxWorksheet getWorksheet(const std::string& name);

        [[nodiscard]] SharedStringTable& getSharedStringTable();

        void writeSharedStringTable();
    };
} // namespace ExcelFiller
