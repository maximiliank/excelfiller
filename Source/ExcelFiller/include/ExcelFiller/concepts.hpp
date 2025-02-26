#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <variant>

namespace ExcelFiller {
    struct EmptyCell {};
    using CellVariants = std::variant<double, std::string, std::string_view, EmptyCell>;
}
namespace ExcelFiller::Concepts {
    template<typename T>
    concept CellConcept =
            std::same_as<T, double> || std::same_as<T, std::string> || std::same_as<T, std::string_view> ||
            std::same_as<T, EmptyCell> || std::same_as<T, CellVariants>;

} // namespace ExcelFiller::Concepts
namespace ExcelFiller {
    template<::ExcelFiller::Concepts::CellConcept T>
    struct CellValue {
        std::size_t row_;
        std::size_t column_;
        T value_;
    };
    using CellValueVariants = CellValue<CellVariants>;
    using CellValueDoubles = CellValue<double>;
    using CellValueStrings = CellValue<std::string>;
    using CellValuesEmpty = CellValue<EmptyCell>;

} // namespace ExcelFiller