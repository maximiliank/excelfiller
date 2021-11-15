#include "SheetRelations.h"
#include <ostream>

ExcelFiller::SheetRelations::SheetRelations(pugi::xml_node relationShips)
{
    for (auto rel : relationShips.children())
    {
        const auto get = [rel](const char* attributeName) -> std::string_view {
            return rel.attribute(attributeName).value();
        };
        const std::string_view id(get("Id"));
        relations_.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                           std::forward_as_tuple(id, get("Type"), get("Target")));
    }
}
std::ostream& ExcelFiller::operator<<(std::ostream& os, const ExcelFiller::SheetRelations& srs)
{
    for (const auto& [id, relation] : srs.relations_)
        os << "Id=" << id << ", "
           << "Type="
           << static_cast<std::underlying_type_t<SheetRelation::RelationType>>(relation.getType())
           << ", Target=" << relation.getTarget() << '\n';
    return os;
}
