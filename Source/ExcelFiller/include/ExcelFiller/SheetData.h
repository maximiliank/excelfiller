#pragma once

#include "ExcelFiller/SharedStringTable.h"
#include <optional>
#include <pugixml.hpp>
namespace ExcelFiller {
    class ColumnProxy {
        pugi::xml_node currentColumn_;
        std::string rowStr_;

    public:
        explicit ColumnProxy(pugi::xml_node column, std::size_t row);

        void setValue(std::size_t column, double value);

        void setValue(std::size_t column, const std::string& value);
    };

    class RowProxy {
        pugi::xml_node currentRow_;
        std::size_t row_;
        ColumnProxy columnProxy_;

        static std::size_t getRowNumber(pugi::xml_node row);

        void setColumnProxy();

    public:
        explicit RowProxy(pugi::xml_node row);

        void setValue(std::size_t column, double value);

        void setValue(std::size_t column, const std::string& value);

        void setRow(std::size_t row);
    };


    class SheetData {
        pugi::xml_node data_;
        RowProxy rowProxy_;
        std::optional<SharedStringTable>& sharedStringTable_;

    public:
        explicit SheetData(pugi::xml_node data,
                           std::optional<SharedStringTable>& sharedStringTable);

        void setValue(std::size_t row, std::size_t column, double value);

        void setValue(std::size_t row, std::size_t column, const std::string& value);

        [[nodiscard]] const pugi::xml_node& getData() const;
    };
}// namespace ExcelFiller
