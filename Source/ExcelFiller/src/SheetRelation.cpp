#include "ExcelFiller/SheetRelation.h"
#include <fmt/format.h>
#include <stdexcept>

ExcelFiller::SheetRelation::SheetRelation(std::string_view id, std::string_view type, std::string_view target)
    : id_(id), type_([type]() {
          if (auto pos = type.find_last_of('/'); pos != std::string_view::npos)
          {
              ++pos;
              const std::string_view type_str = type.substr(pos, type.length() - pos);
              if (type_str == "worksheet")
              {
                  return RelationType::Worksheet;
              }
              else if (type_str == "sharedStrings")
              {
                  return RelationType::SharedStrings;
              }
              else if (type_str == "styles")
              {
                  return RelationType::Styles;
              }
              else if (type_str == "theme")
              {
                  return RelationType::Theme;
              }
              else if (type_str == "customXml")
              {
                  return RelationType::CustomXml;
              }
              else if (type_str == "calcChain")
              {
                  return RelationType::CalculationChain;
              }
              else
              {
                  return RelationType::Unknown;
              }
          }
          else
          {
              throw std::runtime_error(fmt::format("Ill-formed type string in relation entry: {}", type));
          }
      }()),
      target_(target)
{}
const std::string& ExcelFiller::SheetRelation::getId() const
{
    return id_;
}
ExcelFiller::SheetRelation::RelationType ExcelFiller::SheetRelation::getType() const
{
    return type_;
}
const std::string& ExcelFiller::SheetRelation::getTarget() const
{
    return target_;
}
