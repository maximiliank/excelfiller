#pragma once
#include "ExcelFiller/SheetRelation.h"
#include "ExcelFiller/hashmap.hpp"
#include <iosfwd>
#include <pugixml.hpp>

namespace ExcelFiller {
    class SheetRelations {
        friend class Workbook;

        HashMap<std::string, SheetRelation> relations_;

    public:
        explicit SheetRelations(pugi::xml_node relationShips);

        friend std::ostream& operator<<(std::ostream& os, const SheetRelations& srs);
    };
    std::ostream& operator<<(std::ostream& os, const SheetRelations& srs);
}// namespace ExcelFiller
