#include "ExcelFiller/SheetData.h"
#include <fmt/format.h>
#include <string_view>
#include <vector>

ExcelFiller::SheetData::SheetData(pugi::xml_node data) : data_(data), rowProxy_(data_.first_child())
{}

void ExcelFiller::SheetData::setValue(const std::size_t row, const std::size_t column,
                                      const double value)
{
    rowProxy_.setRow(row);
    rowProxy_.setValue(column, value);
}
const pugi::xml_node& ExcelFiller::SheetData::getData() const
{
    return data_;
}

ExcelFiller::RowProxy::RowProxy(const pugi::xml_node row)
    : currentRow_(row), row_(getRowNumber(currentRow_)), columnProxy_(row.first_child(), row_)
{}
void ExcelFiller::RowProxy::setRow(const std::size_t row)
{
    if (row != row_)
    {
        while (row != row_)
        {
            currentRow_ = currentRow_.next_sibling();
            if (currentRow_.empty())
                throw std::runtime_error(fmt::format("Could not find row {}", row));
            row_ = getRowNumber(currentRow_);
        }
        setColumnProxy();
    }
}
std::size_t ExcelFiller::RowProxy::getRowNumber(const pugi::xml_node row)
{
    return row.attribute("r").as_ullong();
}

void ExcelFiller::RowProxy::setValue(std::size_t column, double value)
{
    columnProxy_.setValue(column, value);
}
void ExcelFiller::RowProxy::setValue(std::size_t column, const std::string& value)
{
    columnProxy_.setValue(column, value);
}
void ExcelFiller::RowProxy::setColumnProxy()
{
    columnProxy_ = ColumnProxy(currentRow_.first_child(), row_);
}

ExcelFiller::ColumnProxy::ColumnProxy(pugi::xml_node column, const std::size_t row)
    : currentColumn_(column), rowStr_(std::to_string(row))
{}

namespace ExcelFiller {
    namespace {
        constexpr auto toBase26 = [](std::size_t col) {
            std::string str;
            while (col > 0)
            {
                str = static_cast<char>('A' + (col - 1) % 26) + str;
                col = (col - 1) / 26;
            }
            return str;
        };
    }
}
void ExcelFiller::ColumnProxy::setValue(std::size_t column, double value)
{
    const auto cellRef = toBase26(column) + rowStr_;
    while (std::string_view{currentColumn_.attribute("r").value()} != cellRef)
    {
        currentColumn_ = currentColumn_.next_sibling();
        if (currentColumn_.empty())
            throw std::runtime_error(fmt::format("Could not find column {}", column));
    }

    auto valueNode = [this]() {
        auto node = currentColumn_.find_child(
                [](pugi::xml_node cell) { return strcmp(cell.name(), "v") == 0; });

        if (node.empty())
        {
            node = currentColumn_.append_child("v");
        }
        return node;
    }();
    if (auto attr = currentColumn_.attribute("t"); !attr.empty())
        attr.set_value("n");
    else
        currentColumn_.append_attribute("t").set_value("n");
    valueNode.text() = value;
}

void ExcelFiller::SheetData::setValue(std::size_t row, std::size_t column, const std::string& value)
{
    RowProxy rowProxy(data_.first_child());
    rowProxy.setRow(row);
    rowProxy.setValue(column, value);
}
void ExcelFiller::ColumnProxy::setValue(std::size_t column, const std::string& value)
{
    const auto cellRef = toBase26(column) + rowStr_;
    while (std::string_view{currentColumn_.attribute("r").value()} != cellRef)
    {
        currentColumn_ = currentColumn_.next_sibling();
        if (currentColumn_.empty())
            throw std::runtime_error(fmt::format("Could not find column {}", column));
    }

    if (auto attr = currentColumn_.attribute("t"); !attr.empty())
        attr.set_value("inlineStr");
    else
        currentColumn_.append_attribute("t").set_value("inlineStr");

    currentColumn_.remove_children();
    auto is = currentColumn_.append_child("is");
    auto node = is.append_child("t");

    static std::vector<std::string> stringCache_;
    const auto& valueCache = stringCache_.emplace_back(value);
    node.text() = valueCache.c_str();
}
