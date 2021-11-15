import json
import sys

import openpyxl as xl
from typing import Dict, List
# monkey-patch xfid problems in file crated by open office, see
# https://foss.heptapod.net/openpyxl/openpyxl/-/issues/1579
from openpyxl.styles import stylesheet
from openpyxl.styles.numbers import BUILTIN_FORMATS, BUILTIN_FORMATS_MAX_SIZE


def _expand_named_style(self, named_style):
    """
    Monkey-patched bind format definitions for a named style from the associated style
    record
    """
    try:
        xf = self.cellStyleXfs[named_style.xfId]
        print(f'   ok xfid {named_style.xfId}')
    except:
        print(f'wrong xfid {named_style.xfId}')
        return  # WORKAROUND for faulty Excel sheets
    named_style.font = self.fonts[xf.fontId]
    named_style.fill = self.fills[xf.fillId]
    named_style.border = self.borders[xf.borderId]
    if xf.numFmtId < BUILTIN_FORMATS_MAX_SIZE:
        formats = BUILTIN_FORMATS
    else:
        formats = self.custom_formats
    if xf.numFmtId in formats:
        named_style.number_format = formats[xf.numFmtId]
    if xf.alignment:
        named_style.alignment = xf.alignment
    if xf.protection:
        named_style.protection = xf.protection


stylesheet.Stylesheet._expand_named_style = _expand_named_style  # monkey-patch


class ExcelTemplateParser:
    def __init__(self, filename: str):
        self._workbook = xl.load_workbook(filename, data_only=True)

    def extract_white_cells(self, filename: str, sheet_data: Dict[str, str]) -> None:
        data: Dict[str, Dict[str, List[int]]] = dict()
        for sheet_name, data_range in sheet_data.items():
            rows: List[int] = []
            columns: List[int] = []
            for row in self._workbook[sheet_name][data_range]:
                for cell in row:
                    # identifier for white cells in the EBA template
                    if cell.fill.fgColor.tint == 0.0:
                        rows.append(cell.row)
                        columns.append(cell.col_idx)
                    else:
                        print(f'found color cell {cell.row} {cell.col_idx}')
            data[sheet_name] = {'rows': rows, 'columns': columns}
        with open(filename, 'w') as f:
            json.dump(data, f)


if __name__ == '__main__':
    args = sys.argv[1:]

    if len(args) != 2:
        raise Exception(f"{sys.argv[0]} required 2 arguments: <inputfile> <outputfile>")
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    parser = ExcelTemplateParser(input_file)
    parser.extract_white_cells(output_file,
                               {"CSV_CR_SCEN": "N9:CJ7688", "CSV_CR_REA": "M8:Z6727", "CSV_CR_COVID19": "O14:BC6733"})
