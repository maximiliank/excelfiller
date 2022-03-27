#include "ExcelFiller/xlsxWorkbook.h"
#include "get_file_path.hpp"
#include <filesystem>
#include <random>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <unordered_set>

struct CellValue {
    size_t row_;
    size_t column_;
    double value_;
};

using Results = std::vector<CellValue>;
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

int main()
{
    try
    {
        spdlog::set_pattern("[%^%l%$] %v");
        const std::string filenameOriginal{TestData::getFilePath("example.xlsx")};

        const std::unordered_map<std::string, Results> data = {
                {"Sheet1", createRandomValues(2, 2)},
                {"Sheet2", createRandomValues(2, 2)},
                {"Sheet3", createRandomValues(2, 2)}};


        const std::string targetFilename{"example_filled.xlsx"};
        spdlog::stopwatch swAll;
        std::filesystem::copy(filenameOriginal, targetFilename,
                              std::filesystem::copy_options::overwrite_existing);

        ExcelFiller::XlsxWorkbook wb(targetFilename);
        for (const auto& [sheetName, values] : data)
        {
            spdlog::stopwatch sw;
            auto sheet = wb.getWorksheet(sheetName);
            auto sheetData = sheet.getSheetData();

            for (const auto& cell : values)
                sheetData.setValue(cell.row_, cell.column_, cell.value_);
            sheetData.setValue(1, 1, "TEST");
            sheet.save();

            spdlog::info("Wrote {} cells to sheet {} in {:.4} seconds.", values.size(), sheetName,
                         sw);
        }
        spdlog::info("Wrote excel file {} in {:.4} seconds.", targetFilename, swAll);
        return 0;
    }
    catch (const std::exception& ex)
    {
        spdlog::error(ex.what());
        return 1;
    }
}
