#include "ExcelFiller/SheetData.h"
#include "ExcelFiller/xlsxWorkbook.h"
#include "get_file_path.hpp"
#include <filesystem>
#include <random>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <string_view>
#include <unistd.h>

template<ExcelFiller::CellConcept T>
struct CellValue {
    size_t row_;
    size_t column_;
    T value_;
};


using Results = std::vector<CellValue<double>>;
Results createRandomValues(const std::size_t rows, const std::size_t columns)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 1000.0);

    Results values;
    values.reserve(rows * columns);

    for (std::size_t i = 1; i <= rows; ++i)
    {
        for (std::size_t j = 1; j <= columns; ++j)
        {
            values.emplace_back(i, j, dis(gen));
        }
    }
    return values;
}
std::vector<CellValue<std::string_view>> createStrings(const std::size_t rows,
                                                       const std::size_t columns)
{
    static std::vector<std::string> values;
    values.reserve(rows * columns);

    std::vector<CellValue<std::string_view>> ret;
    ret.reserve(rows * columns);

    for (std::size_t i = 1; i <= rows; ++i)
    {
        for (std::size_t j = 1; j <= columns; ++j)
        {
            auto c = static_cast<char>('A' + i * j - 1);
            values.emplace_back(std::string{c});
            ret.emplace_back(i, j, values.back());
        }
    }
    return ret;
}
std::vector<CellValue<ExcelFiller::CellVariants>> createVariants(const std::size_t rows,
                                                                 const std::size_t columns)
{
    static std::vector<std::string> strings;
    strings.reserve(rows * columns);

    std::vector<CellValue<ExcelFiller::CellVariants>> ret;
    ret.reserve(rows * columns);

    for (std::size_t i = 1; i <= rows; ++i)
    {
        for (std::size_t j = 1; j <= columns; ++j)
        {
            if (i == j)
            {
                ret.emplace_back(i, j, static_cast<double>(i + j));
            }
            else
            {
                auto c = static_cast<char>('A' + i * j - 1);
                strings.emplace_back(std::string{c});
                ret.emplace_back(i, j, strings.back());
            }
        }
    }
    return ret;
}

template<ExcelFiller::CellConcept T>
void writeSheet(ExcelFiller::XlsxWorkbook& wb, const std::string& sheetName,
                std::vector<CellValue<T>> values)
{
    spdlog::stopwatch sw;
    auto sheet = wb.getWorksheet(sheetName);
    auto sheetData = sheet.getSheetData();

    for (const auto& cell : values)
        sheetData.setValue(cell.row_, cell.column_, cell.value_);


    sheet.save();

    spdlog::info("Wrote {} cells to sheet {} in {:.4} seconds.", values.size(), sheetName, sw);
}

int main()
{
    try
    {
        using namespace std::string_view_literals;
        spdlog::set_pattern("[%^%l%$] %v");
        const std::string filenameOriginal{TestData::getFilePath("example.xlsx")};

        const std::string targetFilename{"example_filled.xlsx"};
        spdlog::stopwatch swAll;
        std::filesystem::copy(filenameOriginal, targetFilename,
                              std::filesystem::copy_options::overwrite_existing);

        ExcelFiller::XlsxWorkbook wb(targetFilename);
        writeSheet(wb, "Sheet1", createRandomValues(2, 2));
        writeSheet(wb, "Sheet2", createStrings(2, 2));
        writeSheet(wb, "Sheet3", createVariants(2, 2));
        wb.writeSharedStringTable();
        spdlog::info("Wrote excel file {} in {:.4} seconds.", targetFilename, swAll);
        return 0;
    }
    catch (const std::exception& ex)
    {
        spdlog::error(ex.what());
        return 1;
    }
}
