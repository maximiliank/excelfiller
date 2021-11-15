#include "ExcelFiller/workbook.h"
#include "SheetRelations.h"
#include <fmt/format.h>
#include <ostream>

ExcelFiller::Workbook::Workbook(pugi::xml_node workbook, ExcelFiller::SheetRelations&& relations)
{
    for (auto sheet : workbook.child("sheets").children())
    {
        const auto get = [sheet](const char* attributeName) -> std::string_view {
            return sheet.attribute(attributeName).value();
        };
        const std::string_view name(get("name"));
        const std::string rid(get("r:id"));

        auto& rels = relations.relations_;
        if (auto it = rels.find(rid); it != rels.end())
            sheetRelations_.emplace(name, std::move(it->second));
        else
            throw std::runtime_error(fmt::format(
                    "Could not find r:id {} in SheetRelations for sheet {}", rid, name));
    }
}

std::ostream& ExcelFiller::operator<<(std::ostream& os, const Workbook& wb)
{
    for (const auto& [name, relation] : wb.sheetRelations_)
        os << "Name=" << name << ", Id=" << relation.getId() << ", "
           << "Type="
           << static_cast<std::underlying_type_t<SheetRelation::RelationType>>(relation.getType())
           << ", Target=" << relation.getTarget() << '\n';
    return os;
}
const std::string& ExcelFiller::Workbook::getSheetXmlName(const std::string& name) const
{
    if (auto it = sheetRelations_.find(name); it != sheetRelations_.end())
        return it->second.getTarget();
    else
        throw std::runtime_error(fmt::format("Could not find sheet with name {}", name));
}
