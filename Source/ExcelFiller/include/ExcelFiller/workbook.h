#pragma once
#include "SheetRelation.h"
#include "hashmap.hpp"
#include <iosfwd>
#include <pugixml.hpp>
#include <string>

namespace ExcelFiller {
    class SheetRelations;

    class Workbook {

        HashMap<std::string, SheetRelation> sheetRelations_;

        friend std::ostream& operator<<(std::ostream& os, const Workbook& wb);

    public:
        explicit Workbook(pugi::xml_node workbook, SheetRelations&& relations);

        [[nodiscard]] const std::string& getSheetXmlName(const std::string& name) const;
    };

    std::ostream& operator<<(std::ostream& os, const Workbook& wb);
}// namespace ExcelFiller
