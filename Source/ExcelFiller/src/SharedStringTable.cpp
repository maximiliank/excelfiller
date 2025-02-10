#include "ExcelFiller/SharedStringTable.h"

#include "ExcelFiller/xlsxWorkbook.h"
#include <spdlog/spdlog.h>

ExcelFiller::SharedStringTable::SharedStringTable(pugi::xml_document&& sharedStringTable,
                                                  XlsxWorkbook& workbook)
    : sharedStringTable_(std::move(sharedStringTable)), workbook_(workbook)
{
    pugi::xml_node sst = sharedStringTable_.child("sst");
    pugi::xml_attribute countAttr = sst.attribute("count");
    if (countAttr)
    {
        stringIndexCount_ = countAttr.as_uint();
        //sharedStrings_.reserve(count);
    }

    // Iterate through <si> elements
    std::size_t counter{0};
    for (pugi::xml_node si = sst.child("si"); si; si = si.next_sibling("si"))
    {
        std::string text;

        pugi::xml_node t = si.child("t");
        if (t)
        {
            text = t.child_value();
        }
        else
        {
            // Handle rich text nodes where multiple <r><t> exist
            for (pugi::xml_node r = si.child("r"); r; r = r.next_sibling("r"))
            {
                pugi::xml_node t_rich = r.child("t");
                if (t_rich)
                {
                    text += t_rich.child_value();
                }
            }
        }

        existingSharedStrings_.emplace(std::move(text), counter++);
    }
    if (existingSharedStrings_.size() != stringIndexCount_)
    {
        spdlog::warn("SharedStringTable count does not match number of strings stored {} != {}",
                     existingSharedStrings_.size(), stringIndexCount_);
    }
}

std::size_t ExcelFiller::SharedStringTable::getSharedStringIndex(const std::string_view item)
{
    if (auto itExisting = existingSharedStrings_.find(item);
        itExisting != existingSharedStrings_.end())
    {
        return itExisting->second;
    }
    else if (auto it = newlyAddedStrings_.find(item); it != newlyAddedStrings_.end())
    {
        return it->second;
    }
    newlyAddedStrings_.emplace(item, stringIndexCount_);
    return stringIndexCount_++;
}

void ExcelFiller::SharedStringTable::writeSharedStringTable()
{
    if (!newlyAddedStrings_.empty())
    {
        pugi::xml_node sst = sharedStringTable_.child("sst");
        for (const auto& [str, idx] : newlyAddedStrings_)
        {
            pugi::xml_node si = sst.append_child("si");
            pugi::xml_node t = si.append_child("t");
            t.append_attribute("xml:space") = "preserve";
            t.text().set(str.c_str());
        }

        const auto updateCounts =
                [&sst, addedStrings = newlyAddedStrings_.size()](const char* counterName) {
                    pugi::xml_attribute countAttr = sst.attribute(counterName);
                    if (countAttr)
                    {
                        const auto oldCount = countAttr.as_uint();
                        countAttr.set_value(oldCount + addedStrings);
                    }
                };
        updateCounts("count");
        updateCounts("uniqueCount");

        workbook_.writeXMLFile("xl/sharedStrings.xml", sharedStringTable_);
    }
}