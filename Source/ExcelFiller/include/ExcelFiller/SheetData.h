#pragma once

#include "ExcelFiller/SharedStringTable.h"
#include <concepts>
#include <pugixml.hpp>
#include <variant>
namespace ExcelFiller {
    using CellVariants = std::variant<double, std::string_view>;
    template<typename T>
    concept CellConcept = std::same_as<T, double> or std::same_as<T, std::string_view> or
                          std::same_as<T, CellVariants>;
    class ColumnProxy {
        pugi::xml_node currentColumn_;
        std::string rowStr_;

    public:
        explicit ColumnProxy(pugi::xml_node column, std::size_t row);

        void setValue(std::size_t column, double value, SharedStringTable& sharedStringTable);

        void setValue(std::size_t column, std::string_view value,
                      SharedStringTable& sharedStringTable);

        void setValue(std::size_t column, CellVariants value, SharedStringTable& sharedStringTable);
    };

    class RowProxy {
        pugi::xml_node currentRow_;
        std::size_t row_;
        ColumnProxy columnProxy_;

        static std::size_t getRowNumber(pugi::xml_node row);

        void setColumnProxy();

    public:
        explicit RowProxy(pugi::xml_node row);

        template<CellConcept T>
        void setValue(std::size_t column, T value, SharedStringTable& sharedStringTable)
        {
            columnProxy_.setValue(column, value, sharedStringTable);
        }

        void setRow(std::size_t row);
    };


    class SheetData {
        pugi::xml_node data_;
        RowProxy rowProxy_;
        SharedStringTable& sharedStringTable_;

    public:
        explicit SheetData(pugi::xml_node data, SharedStringTable& sharedStringTable);

        template<CellConcept T>
        void setValue(std::size_t row, std::size_t column, T value)
        {
            rowProxy_.setRow(row);
            rowProxy_.setValue(column, value, sharedStringTable_);
        }

        [[nodiscard]] const pugi::xml_node& getData() const;
    };
}// namespace ExcelFiller
