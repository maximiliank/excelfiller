ExcelFiller
===========

`ExcelFiller` is a library which aims to fill `double` precision floating point values into an Excel template. It is
designed to add values per sheet for coordinates given by row and column (one based) indices and writes them into the
Excel template. Currently, it only supports adding values in a forward iteration manner to a sheet, i.e. your data
should be sorted by row and column number.

It depends on [pugixml](https://github.com/zeux/pugixml "pugixml")
and [{fmt}](https://github.com/fmtlib/fmt "fmt") as libraries. Internally it uses
uses [kuba-- zip](https://github.com/kuba--/zip "zip") as zip library which is included in the repository.

> **WARNING**: This is not an Excel library as it only handles a very specific use case. It is not even supporting strings, hence it is not even loading the shared string table inside the Excel archive.

Also note that it does not add cells to the Excel template, i.e. the cell must be already available in the internal xml
of the sheet.

If you want to build and install the library you have to run

```shell script
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=OFF -DUSE_CONAN=ON -DCMAKE_INSTALL_PREFIX=<your install path>
make
make install
```

By default it uses the conan cmake integration to provide the required packages.

API
---

The API basically consists of opening the workbook, getting the worksheet and setting values via the `SheetData` class,
e.g.:

```c++
#include <vector>
#include <algorithm>
#include <tuple>
#include "ExcelFiller/xlsxWorkbook.h"

struct CellValue {
    size_t row_;
    size_t column_;
    double value_;
};

int main() {
    std::vector<CellValue> values = { /* initialize your data here */};
    // Make sure the values are sorted by row and column
    std::sort(std::begin(cells), std::end(cells), [](const CellValue& lhs, const CellValue& rhs) {
        return std::tie(lhs.row_, lhs.column_) < std::tie(rhs.row_, rhs.column_);
    });
    
    // Open the workbook
    ExcelFiller::XlsxWorkbook wb("YourExcel.xlsx");
    // query the sheet and get a SheetData object
    auto sheet = wb.getWorksheet("Sheet1");
    auto sheetData = sheet.getSheetData();
    
    // Iterate over your values and call setValue on the SheetData object 
    for (const auto& cell : values)
        sheetData.setValue(cell.row_, cell.column_, cell.value_);
    // Save the sheet
    sheet.save();
    
    return 0;
}
```

See the `examples` directory for more details.

Examples
--------

The `EBABenchmark` example downloads the EBA template from the 2021 stress test and converts it to `.xlsx` format. This
step takes quite a time with `LibreOffice`. When it is run, random samples are created and filled into the target cells
for selected sheets. The output of the benchmark looks like

```shell
[info] Wrote 92173 cells to sheet CSV_CR_SCEN in 1.242 seconds.
[info] Wrote 36612 cells to sheet CSV_CR_REA in 0.2763 seconds.
[info] Wrote 22200 cells to sheet CSV_CR_COVID19 in 0.3601 seconds.
[info] Wrote excel file EBA_2021_EU-wide_stress_test_Templates_v0_filled.xlsx in 1.920 seconds.
```

The above timings were produced using gcc 11.2 running on an Intel(R) Core(TM) i7-7820X CPU @ 3.60GHz.

The examples are using [spdlog](https://github.com/gabime/spdlog) for logging purposes
and [nlohman json](https://github.com/nlohmann/json) in order to load the target cell JSON file required for the
EBABenchmark example.

Notes
-----

The `CMake` setup was taken from [here](https://github.com/cpp-best-practices/cpp_starter_project).
