#include "ExcelFiller/xlsxWorksheet.h"
#include "ExcelFiller/xlsxWorkbook.h"
#include <fmt/format.h>

ExcelFiller::XlsxWorksheet::XlsxWorksheet(pugi::xml_document&& doc, XlsxWorkbook& workbook, std::string xmlName)
    : doc_(std::move(doc)), workbook_(workbook), xmlName_(std::move(xmlName))
{}
ExcelFiller::SheetData ExcelFiller::XlsxWorksheet::getSheetData()
{
    return ExcelFiller::SheetData(doc_.child("worksheet").child("sheetData"), workbook_.getSharedStringTable());
}
void ExcelFiller::XlsxWorksheet::save()
{
    workbook_.writeXMLFile(fmt::format("xl/{}", xmlName_), doc_);
}
