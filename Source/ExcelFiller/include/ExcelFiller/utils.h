#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace ExcelFiller::Utils {
    struct string_hash {
        using hash_type = std::hash<std::string_view>;
        using is_transparent = void;

        [[nodiscard]] inline std::size_t operator()(const char* str) const
        {
            return hash_type{}(str);
        }
        [[nodiscard]] inline std::size_t operator()(std::string_view str) const
        {
            return hash_type{}(str);
        }
        [[nodiscard]] inline std::size_t operator()(std::string const& str) const
        {
            return hash_type{}(str);
        }
    };

}// namespace ExcelFiller::Utils