#include "ExcelFiller/xlsxWorkbook.h"
#include "SheetRelations.h"
#include <fmt/format.h>
#include <pugixml.hpp>

ExcelFiller::XlsxWorkbook::XlsxWorkbook(const std::string& filename)
    : ZipXMLHelper(filename), workbook_([this]() {
          auto doc = ZipXMLHelper::loadXMLFile("xl/_rels/workbook.xml.rels");
          auto relations = SheetRelations(doc.child("Relationships"));
          doc = ZipXMLHelper::loadXMLFile("xl/workbook.xml");
          return Workbook(doc.child("workbook"), std::move(relations));
      }())
{}

ExcelFiller::XlsxWorksheet ExcelFiller::XlsxWorkbook::getWorksheet(const std::string& name)
{
    const auto& xmlName = workbook_.getSheetXmlName(name);
    return XlsxWorksheet(ZipXMLHelper::loadXMLFile(fmt::format("xl/{}", xmlName)), *this, xmlName);
}
