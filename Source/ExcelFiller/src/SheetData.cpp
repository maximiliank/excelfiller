#include "ExcelFiller/SheetData.h"
#include <cmath>
#include <fmt/format.h>
#include <string_view>
#include <spdlog/spdlog.h>

ExcelFiller::SheetData::SheetData(pugi::xml_node data, SharedStringTable& sharedStringTable)
    : data_(data), rowProxy_(data_.first_child()), sharedStringTable_(sharedStringTable)
{}

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
            {
                throw std::runtime_error(fmt::format("Could not find row {}", row));
            }
            row_ = getRowNumber(currentRow_);
        }
        setColumnProxy();
    }
}
std::size_t ExcelFiller::RowProxy::getRowNumber(const pugi::xml_node row)
{
    return row.attribute("r").as_ullong();
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

        pugi::xml_node getValueNode(pugi::xml_node item)
        {
            auto node = item.find_child([](pugi::xml_node cell) { return strcmp(cell.name(), "v") == 0; });

            if (node.empty())
            {
                node = item.append_child("v");
            }
            return node;
        }
        std::string_view getCellRef(const pugi::xml_node& cell)
        {
            if (auto attr = cell.attribute("r"); !attr.empty())
            {
                return attr.value();
            }
            else
            {
                return "Unknown cell";
            }
        }
        void handleAttributeTypeChangeWarning(
                const pugi::xml_node& cell, std::string_view currentAttribute, std::string_view newAttribute)
        {
            if (newAttribute != "e")
            {
                spdlog::warn("Changing attribute type of cell {} from {} to {}", getCellRef(cell), currentAttribute,
                        newAttribute);
            }
        }
        void setAttribute(pugi::xml_node& cell, const char* attributeType)
        {
            pugi::xml_attribute attr = cell.attribute("t");

            if (!attr.empty())
            {
                if (const char* currentAttribute = attr.value(); std::strcmp(currentAttribute, attributeType) != 0)
                {
                    handleAttributeTypeChangeWarning(cell, currentAttribute, attributeType);
                    attr.set_value(attributeType);
                }
            }
            else if (std::strcmp(attributeType, "n") != 0)
            {
                // Only add the attribute if newValue is NOT "n"
                handleAttributeTypeChangeWarning(cell, "n", attributeType);
                cell.append_attribute("t").set_value(attributeType);
            }
        }
        void setDoubleCellValue(pugi::xml_node& cell, double value)
        {
            auto valueNode = getValueNode(cell);
            if (!std::isfinite(value))
            {
                const auto cellRef = getCellRef(cell);
                setAttribute(cell, "e");
                if (std::isinf(value))
                {
                    // +-Infinity -> #DIV/0! (Excel error)
                    spdlog::warn("Setting infinity value in cell {}", cellRef);
                    valueNode.text() = "#DIV/0!";
                }
                else
                {
                    // NaN -> #N/A (Excel error)
                    spdlog::warn("Setting NaN value in cell {}", cellRef);
                    valueNode.text() = "#N/A";
                }
            }
            else
            {
                setAttribute(cell, "n");
                valueNode.text() = value;
            }
        }
    } // namespace
} // namespace ExcelFiller
void ExcelFiller::ColumnProxy::setValue(
        std::size_t column, double value, [[maybe_unused]] SharedStringTable& sharedStringTable)
{
    const auto cellRef = toBase26(column) + rowStr_;
    while (std::string_view{currentColumn_.attribute("r").value()} != cellRef)
    {
        currentColumn_ = currentColumn_.next_sibling();
        if (currentColumn_.empty())
        {
            throw std::runtime_error(fmt::format("Could not find column {}", column));
        }
    }

    setDoubleCellValue(currentColumn_, value);
}
void ExcelFiller::ColumnProxy::setValue(
        std::size_t column, const std::string& value, SharedStringTable& sharedStringTable)
{
    setValue(column, std::string_view{value}, sharedStringTable);
}
void ExcelFiller::ColumnProxy::setValue(
        std::size_t column, const std::string_view value, SharedStringTable& sharedStringTable)
{
    const auto cellRef = toBase26(column) + rowStr_;
    while (std::string_view{currentColumn_.attribute("r").value()} != cellRef)
    {
        currentColumn_ = currentColumn_.next_sibling();
        if (currentColumn_.empty())
        {
            throw std::runtime_error(fmt::format("Could not find column {}", column));
        }
    }

    setAttribute(currentColumn_, "s");

    auto valueNode = getValueNode(currentColumn_);

    const auto idx = sharedStringTable.getSharedStringIndex(value);
    valueNode.text() = idx;
}


void ExcelFiller::ColumnProxy::setValue(
        std::size_t column, const CellVariants& value, SharedStringTable& sharedStringTable)
{
    std::visit([this, column, &sharedStringTable](auto&& arg) { setValue(column, arg, sharedStringTable); }, value);
}