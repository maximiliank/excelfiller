#pragma once
#include "ExcelFiller/SharedStringTable.h"
#include "SheetRelation.h"
#include "hashmap.hpp"
#include <iosfwd>
#include <optional>
#include <pugixml.hpp>
#include <string>
namespace ExcelFiller {
    class SheetRelations;

    class Workbook {

        HashMap<std::string, SheetRelation> sheetRelations_;
        std::optional<SharedStringTable> sharedStringTable_;

        friend std::ostream& operator<<(std::ostream& os, const Workbook& wb);

    public:
        explicit Workbook(pugi::xml_node workbook, SheetRelations&& relations,
                          std::optional<SharedStringTable>&& sharedStringTable);

        [[nodiscard]] const std::string& getSheetXmlName(const std::string& name) const;

        void writeSharedStringTable();

        [[nodiscard]] std::optional<SharedStringTable>& getSharedStringTable();
    };

    std::ostream& operator<<(std::ostream& os, const Workbook& wb);
}// namespace ExcelFiller
