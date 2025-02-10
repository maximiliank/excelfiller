#pragma once

#include "ExcelFiller/SharedStringTable.h"
#include "workbook.h"
#include "xlsxWorksheet.h"
#include "zipxmlhelper.h"
#include <optional>
#include <string>
namespace ExcelFiller {
    class XlsxWorkbook : ZipXMLHelper {
        Workbook workbook_;

    public:
        using ZipXMLHelper::writeXMLFile;

        explicit XlsxWorkbook(const std::string& filename, bool loadSharedStrings);

        [[nodiscard]] XlsxWorksheet getWorksheet(const std::string& name);

        [[nodiscard]] std::optional<SharedStringTable>& getSharedStringTable();

        void writeSharedStringTable();
    };
}// namespace ExcelFiller
