#!/usr/bin/env python3
"""
GGWP — Glyph Generator Workflow Protocol
Default configuration for glyph sheet generation and font building.

This is the default config. Copy and modify for your own font project.
Pass your custom config to generate_sheet.py:
    python3 generate_sheet.py my_font_config.py

Every field is documented. Required fields are marked [REQUIRED].
Optional fields have sensible defaults.
"""

CONFIG = {
    # ------------------------------------------------------------------
    # Font Metadata [REQUIRED]
    # ------------------------------------------------------------------
    "family_name": "Display Mono",
    "version": "1.0",
    "copyright": (
        "MMXXVI \u00a9 JUBILANT RESEARCH OF ECLECTIC NOVELTY "
        "IN GENERATING MUSIC. ALL RIGHTS RESERVED."
    ),
    "trademark": (
        "Display is a registered trademark of \u00a9 JRENG! ALL RIGHTS RESERVED."
    ),
    "vendor": "JRNG",  # 4-char vendor ID (achVendID)
    "designer": "JRENG!",
    "description": "Designed for Ephemeral Nexus Display.",
    "url": "https://jrengmusic.com",
    # ------------------------------------------------------------------
    # Font Type [REQUIRED]
    # ------------------------------------------------------------------
    # "monospace" — all glyphs share cell_width as advance width.
    #               No LSB/RSB markers needed.
    # "proportional" — each glyph has its own advance width defined
    #                  by LSB (cyan) and RSB (magenta) markers in the cell.
    "font_type": "monospace",
    # ------------------------------------------------------------------
    # Weights [REQUIRED]
    # Each weight produces one SVG sheet and one TTF.
    # ------------------------------------------------------------------
    "weights": [
        {
            "name": "Book",  # human-readable weight name
            "style": "Book",  # OS/2 subfamily
            "ps_suffix": "Book",  # PostScript name suffix
            "weight_class": 350,  # usWeightClass
            "filename": "GlyphSheet-Book.svg",  # output SVG filename
        },
        {
            "name": "Medium",
            "style": "Medium",
            "ps_suffix": "Medium",
            "weight_class": 500,
            "filename": "GlyphSheet-Medium.svg",
        },
        {
            "name": "Bold",
            "style": "Bold",
            "ps_suffix": "Bold",
            "weight_class": 700,
            "filename": "GlyphSheet-Bold.svg",
        },
    ],
    # ------------------------------------------------------------------
    # Grid Layout [REQUIRED]
    # ------------------------------------------------------------------
    # columns: glyphs per row
    # cell_width: width of the drawing area inside each cell (px)
    # cell_height: height of the drawing area inside each cell (px)
    # col_gap: horizontal gap between cells (px)
    # row_gap: vertical gap between cells (px), includes label row
    # label_height: height reserved for the codepoint label above each cell (px)
    #
    # Total col_step = cell_width + col_gap
    # Total row_step = cell_height + row_gap
    # SVG width  = margin_left + columns * col_step - col_gap + margin_right
    # SVG height = margin_top + rows * row_step - row_gap + margin_bottom
    "grid": {
        "columns": 10,
        "cell_width": 120,
        "cell_height": 223,
        "col_gap": 20,
        "row_gap": 20,
        "label_height": 15,
        "margin_left": 40,
        "margin_top": 60,
        "margin_right": 20,
        "margin_bottom": 40,
    },
    # ------------------------------------------------------------------
    # Reference Lines [REQUIRED]
    # Vertical positions relative to cell top (0 = top of cell_height).
    # These define font metrics. The designer can adjust them in the SVG
    # editor; build_fonts.py reads the actual positions from the SVG.
    #
    # For sheet generation, these are the INITIAL positions.
    # Values are in px from the top of each cell's drawing area.
    # ------------------------------------------------------------------
    "reference_lines": {
        "capline": {"y": 0, "color": "#800080", "label": "Cap Height"},
        "xheight": {"y": 40, "color": "#008000", "label": "x-Height"},
        "baseline": {"y": 145, "color": "#808000", "label": "Baseline"},
        "descender": {"y": 223, "color": "#FF0000", "label": "Descender"},
    },
    # ------------------------------------------------------------------
    # Sidebearing Markers (proportional fonts only)
    # Vertical lines within each cell marking left and right boundaries.
    # Ignored for monospace fonts.
    # ------------------------------------------------------------------
    "sidebearing_markers": {
        "lsb_color": "#00FFFF",  # cyan — left sidebearing
        "rsb_color": "#FF00FF",  # magenta — right sidebearing
    },
    # ------------------------------------------------------------------
    # Sheet Appearance
    # ------------------------------------------------------------------
    "appearance": {
        "background": "#1A1A1A",  # dark grey background
        "cell_border_color": "#333333",  # cell rectangle stroke
        "cell_border_width": 0.5,
        "label_color": "#666666",  # codepoint label text color
        "label_font_size": 7,  # px
        "ref_line_width": 0.5,  # reference line stroke width
        "ref_line_opacity": 0.6,
        "marker_line_width": 0.5,  # LSB/RSB marker stroke width
        "marker_opacity": 0.6,
    },
    # ------------------------------------------------------------------
    # Codepoints [REQUIRED]
    # Flat list of characters to include. Auto-fills left-to-right,
    # top-to-bottom into the grid.
    # ------------------------------------------------------------------
    "codepoints": [
        # ASCII printable (0x21-0x7E) — 94 characters
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
    ],
    # ------------------------------------------------------------------
    # Output Directories
    # ------------------------------------------------------------------
    "output": {
        "sheets_dir": "sheets",  # where blank SVGs go
        "fonts_dir": "fonts",  # where built TTFs go
    },
}
