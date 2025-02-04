#include "ExcelFiller/hashmap.hpp"
#include "ExcelFiller/xlsxWorkbook.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <random>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

struct CellValue {
    size_t row_;
    size_t column_;
    double value_;
};

void sortCells(std::vector<CellValue>& cells)
{
    std::sort(std::begin(cells), std::end(cells), [](const CellValue& lhs, const CellValue& rhs) {
        return std::tie(lhs.row_, lhs.column_) < std::tie(rhs.row_, rhs.column_);
    });
}

ExcelFiller::HashMap<std::string, std::vector<CellValue>> loadTargetCells()
{
    constexpr const char* target_cells_file = "target_cells.json";
    std::ifstream file(target_cells_file);
    nlohmann::json j;
    file >> j;

    ExcelFiller::HashMap<std::string, std::vector<CellValue>> ret;
    for (auto& [key, value] : j.items())
    {
        const auto rows = value["rows"].get<std::vector<std::size_t>>();
        const auto columns = value["columns"].get<std::vector<std::size_t>>();

        if (rows.size() != columns.size())
            throw std::runtime_error(
                    fmt::format("Target cells file {} is malformed", target_cells_file));

        std::vector<CellValue> cells;
        cells.reserve(rows.size());

        for (std::size_t i = 0; i < rows.size(); ++i)
            cells.emplace_back(rows[i], columns[i], -1.);

        ret.emplace(key, std::move(cells));
    }

    return ret;
}

void fillWithRandomValues(ExcelFiller::HashMap<std::string, std::vector<CellValue>>& targets)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 1000.0);
    for (auto& [sheet, cells] : targets)
    {
        for (auto& cell : cells)
            cell.value_ = dis(gen);
    }
}

void writeExcel(const std::string& filenameOriginal, const std::string& targetFilename,
                const ExcelFiller::HashMap<std::string, std::vector<CellValue>>& targets)
{
    spdlog::stopwatch swAll;
    std::filesystem::copy(filenameOriginal, targetFilename,
                          std::filesystem::copy_options::overwrite_existing);

    ExcelFiller::XlsxWorkbook wb(targetFilename, false);

    for (const auto& [sheetName, cells] : targets)
    {
        spdlog::stopwatch sw;

        auto sheet = wb.getWorksheet(sheetName);
        auto sheetData = sheet.getSheetData();

        for (const auto& cell : cells)
            sheetData.setValue(cell.row_, cell.column_, cell.value_);
        sheet.save();

        spdlog::info("Wrote {} cells to sheet {} in {:.4} seconds.", cells.size(), sheetName, sw);
    }
    spdlog::info("Wrote excel file {} in {:.4} seconds.", targetFilename, swAll);
}

int main()
{
    try
    {
        spdlog::set_pattern("[%^%l%$] %v");

        auto targetCells = loadTargetCells();
        fillWithRandomValues(targetCells);
        for (auto& [sheetName, cells] : targetCells)
            sortCells(cells);

        writeExcel("EBA_2021_EU-wide_stress_test_Templates_v0.xlsx",
                   "EBA_2021_EU-wide_stress_test_Templates_v0_filled.xlsx", targetCells);
        return 0;
    }
    catch (const std::exception& ex)
    {
        spdlog::error(ex.what());
        return 1;
    }
}
