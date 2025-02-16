#pragma once

#include <string>
#include <string_view>

namespace ExcelFiller {
    class SheetRelation {
      public:
        enum class RelationType { Worksheet, SharedStrings, Styles, Theme, CustomXml, CalculationChain, Unknown };
        explicit SheetRelation(std::string_view rID, std::string_view type, std::string_view target);
        [[nodiscard]] const std::string& getId() const;
        [[nodiscard]] RelationType getType() const;
        [[nodiscard]] const std::string& getTarget() const;

      private:
        std::string id_;
        RelationType type_;
        std::string target_;
    };
} // namespace ExcelFiller
