#!/usr/bin/env python3
"""
GGWP — GlyphSheet Generation Workflow Protocol
Single source of truth for grid, metrics, and font metadata.
All scripts import from here. No hardcoded derived values elsewhere.

Copy and modify for your own font project.
"""

COLS = 10
MARGIN_TOP = 70
MARGIN_LEFT = 40
CELL_W = 120
CELL_H = 243
GUTTER = 10

COL_STEP = CELL_W + GUTTER
ROW_STEP = CELL_H + GUTTER

REFLINES = [
    (31, "rgb(167,139,250)"),
    (71, "rgb(52,211,153)"),
    (176, "rgb(250,204,21)"),
    (217, "rgb(248,113,113)"),
]

CAPLINE_Y = MARGIN_TOP + 31
BASELINE_Y = MARGIN_TOP + 176

CAP_H = 739
X_H = 552
ASC = 881
DESC = -122
LINE_GAP = 0
ADVANCE_W_FACTOR = 5.125

FAMILY = "My Font"
VERSION = "1.0"
COPYRIGHT = "Copyright 2026 Your Name"
TRADEMARK = ""
VENDOR = "NONE"
DESIGNER = "Your Name"
DESCRIPTION = ""
URL = ""

CHARS = [
    "!",
    '"',
    "#",
    "$",
    "%",
    "&",
    "'",
    "(",
    ")",
    "*",
    "+",
    ",",
    "-",
    ".",
    "/",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    ":",
    ";",
    "<",
    "=",
    ">",
    "?",
    "@",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "[",
    "\\",
    "]",
    "^",
    "_",
    "`",
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
    "h",
    "i",
    "j",
    "k",
    "l",
    "m",
    "n",
    "o",
    "p",
    "q",
    "r",
    "s",
    "t",
    "u",
    "v",
    "w",
    "x",
    "y",
    "z",
    "{",
    "|",
    "}",
    "~",
]

LIGATURES_2 = []
LIGATURES_3 = []

WEIGHTS = [
    {
        "svg": "GlyphSheet-Regular.svg",
        "style": "Regular",
        "full": "My Font Regular",
        "ps": "MyFont-Regular",
        "weight_class": 400,
    },
]

BG_COLOR = "rgb(20,20,20)"
CELL_STROKE = "rgb(50,50,50)"
LABEL_COLOR = "rgb(100,100,100)"
LABEL_FONT_SIZE = 7
GLYPH_FILL = "rgb(224,224,224)"

REFLINE_COLORS = {color for _, color in REFLINES}
