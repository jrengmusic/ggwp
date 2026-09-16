#!/usr/bin/env python3
"""
Generate glyph sheets pre-filled with outlines from existing TTF fonts.
Cell widths derive from source font advance widths (proportional layout).
Produces SVGs compatible with build_fonts.py.
"""

import math
import os
import xml.etree.ElementTree as ET

from fontTools.ttLib import TTFont
from fontTools.pens.svgPathPen import SVGPathPen
from fontTools.pens.transformPen import TransformPen

from sheet_config import (
    ARROWS,
    BG_COLOR,
    CELL_H,
    CELL_STROKE,
    CELL_W,
    CHARS,
    COLS,
    CAPLINE_Y,
    BASELINE_Y,
    CAP_H,
    GUTTER,
    GLYPH_FILL,
    LABEL_COLOR,
    LABEL_FONT_SIZE,
    LIGATURES_2,
    LIGATURES_3,
    MARGIN_LEFT,
    MARGIN_TOP,
    REFLINES,
    ROW_STEP,
    WEIGHTS,
)

ET.register_namespace("", "http://www.w3.org/2000/svg")

SRC_DIR = "/Users/jreng/Documents/Poems/dev/jam/jam_fonts/display"
SRC_MAP = {
    "Book": "DisplayBook.ttf",
    "Medium": "DisplayMedium.ttf",
    "Bold": "DisplayBold.ttf",
}

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(BASE_DIR, "sheets")
os.makedirs(OUT_DIR, exist_ok=True)

SCALE = CAP_H / (BASELINE_Y - CAPLINE_Y)
SCALE_INV = 1.0 / SCALE
BASELINE_OFFSET = BASELINE_Y - MARGIN_TOP


def ntos(v):
    """Number to string — 2 decimal places, trailing zeros stripped."""
    s = f"{v:.2f}"
    if "." in s:
        s = s.rstrip("0").rstrip(".")
    return s


def char_id(ch):
    return "_" + "_".join(f"{ord(c):04X}" for c in ch)


def get_advance(tt, ch):
    """Get advance width in SVG pixels for a character from source font."""
    cmap = tt.getBestCmap()
    cp = ord(ch) if len(ch) == 1 else None
    if cp is None or cp not in cmap:
        return CELL_W
    glyph_name = cmap[cp]
    advance = tt["hmtx"][glyph_name][0]
    return round(advance * SCALE_INV)


def compute_layout(chars, widths):
    """Compute (ch, x, y, w) for each character. Variable-width, COLS per row."""
    layout = []
    for idx, ch in enumerate(chars):
        col = idx % COLS
        row = idx // COLS
        y = MARGIN_TOP + row * ROW_STEP

        if col == 0:
            x = MARGIN_LEFT
        else:
            _, prev_x, _, prev_w = layout[-1]
            x = prev_x + prev_w + GUTTER

        layout.append((ch, x, y, widths[ch]))
    return layout


def emit_guides(guides_g, x, y, w, label):
    text = ET.SubElement(guides_g, "text")
    text.set("x", str(x + 2))
    text.set("y", str(y + 8))
    text.set("style", f"font-size:{LABEL_FONT_SIZE}px;fill:{LABEL_COLOR}")
    text.text = label

    for dy, color in REFLINES:
        ry = y + dy
        path = ET.SubElement(guides_g, "path")
        path.set("d", f"M {x},{ry} L {x + w},{ry}")
        path.set("style", f"fill:none;stroke:{color};stroke-width:0.5")

    cx = x + w // 2
    center = ET.SubElement(guides_g, "path")
    center.set("d", f"M {cx},{y} L {cx},{y + CELL_H}")
    center.set(
        "style",
        "fill:none;stroke:rgb(68,68,68);stroke-width:0.5px;stroke-dasharray:2,3",
    )


def emit_cell_group(svg_root, glyph_id, x, y, w):
    g = ET.SubElement(svg_root, "g")
    g.set("id", glyph_id)
    rect = ET.SubElement(g, "rect")
    rect.set("x", str(x))
    rect.set("y", str(y))
    rect.set("width", str(w))
    rect.set("height", str(CELL_H))
    rect.set("style", f"fill:none;stroke:{CELL_STROKE};stroke-width:0.5")
    return g


def add_glyph_artwork(tt, codepoint, cell_x, cell_y, cell_w, g_elem):
    """Extract glyph outline from TTFont, transform to SVG coords, add as <path>."""
    cmap = tt.getBestCmap()
    if codepoint not in cmap:
        return False

    glyph_name = cmap[codepoint]
    glyph_set = tt.getGlyphSet()
    baseline_svg_y = cell_y + BASELINE_OFFSET

    xform = (SCALE_INV, 0, 0, -SCALE_INV, cell_x, baseline_svg_y)

    svg_pen = SVGPathPen(glyph_set, ntos=ntos)
    t_pen = TransformPen(svg_pen, xform)
    glyph_set[glyph_name].draw(t_pen)

    d = svg_pen.getCommands()
    if not d:
        return False

    path_elem = ET.SubElement(g_elem, "path")
    path_elem.set("d", d)
    path_elem.set("fill", GLYPH_FILL)
    return True


def build_prefilled_svg(tt):
    """Build complete SVG with guides and pre-filled glyph artwork."""
    char_widths = {ch: get_advance(tt, ch) for ch in CHARS}
    char_layout = compute_layout(CHARS, char_widths)
    char_rows = math.ceil(len(CHARS) / COLS)

    lig2_cols = COLS // 2
    lig2_rows = math.ceil(len(LIGATURES_2) / lig2_cols) if LIGATURES_2 else 0

    lig3_cols = COLS // 3
    lig3_rows = math.ceil(len(LIGATURES_3) / lig3_cols) if LIGATURES_3 else 0

    arrow_widths = {ch: get_advance(tt, ch) for ch in ARROWS}
    arrow_layout = compute_layout(ARROWS, arrow_widths) if ARROWS else []
    arrow_rows = math.ceil(len(ARROWS) / COLS) if ARROWS else 0

    total_rows = char_rows + lig2_rows + lig3_rows + arrow_rows

    max_x_extent = MARGIN_LEFT
    for _, x, _, w in char_layout:
        max_x_extent = max(max_x_extent, x + w)
    for _, x, _, w in arrow_layout:
        max_x_extent = max(max_x_extent, x + w)

    svg_w = max_x_extent + MARGIN_LEFT
    svg_h = MARGIN_TOP + total_rows * ROW_STEP - GUTTER + 20

    svg = ET.Element("svg")
    svg.set("xmlns", "http://www.w3.org/2000/svg")
    svg.set("width", str(svg_w))
    svg.set("height", str(svg_h))

    bg = ET.SubElement(svg, "rect")
    bg.set("width", str(svg_w))
    bg.set("height", str(svg_h))
    bg.set("fill", BG_COLOR)

    guides_g = ET.SubElement(svg, "g")
    guides_g.set("id", "guides")

    for ch, x, y, w in char_layout:
        emit_guides(guides_g, x, y, w, ch)

    lig2_w = CELL_W * 2 + GUTTER
    lig2_row_offset = char_rows
    for idx, lig in enumerate(LIGATURES_2):
        row = idx // lig2_cols
        col = idx % lig2_cols
        x = MARGIN_LEFT + col * (lig2_w + GUTTER)
        y = MARGIN_TOP + (lig2_row_offset + row) * ROW_STEP
        emit_guides(guides_g, x, y, lig2_w, lig)

    lig3_w = CELL_W * 3 + GUTTER * 2
    lig3_row_offset = char_rows + lig2_rows
    for idx, lig in enumerate(LIGATURES_3):
        row = idx // lig3_cols
        col = idx % lig3_cols
        x = MARGIN_LEFT + col * (lig3_w + GUTTER)
        y = MARGIN_TOP + (lig3_row_offset + row) * ROW_STEP
        emit_guides(guides_g, x, y, lig3_w, lig)

    arrow_y_base = MARGIN_TOP + (char_rows + lig2_rows + lig3_rows) * ROW_STEP
    for ch, x, y_rel, w in arrow_layout:
        row = (y_rel - MARGIN_TOP) // ROW_STEP
        y = arrow_y_base + row * ROW_STEP
        emit_guides(guides_g, x, y, w, ch)

    filled = 0

    for ch, x, y, w in char_layout:
        g = emit_cell_group(svg, char_id(ch), x, y, w)
        if add_glyph_artwork(tt, ord(ch), x, y, w, g):
            filled += 1

    for idx, lig in enumerate(LIGATURES_2):
        row = idx // lig2_cols
        col = idx % lig2_cols
        x = MARGIN_LEFT + col * (lig2_w + GUTTER)
        y = MARGIN_TOP + (lig2_row_offset + row) * ROW_STEP
        emit_cell_group(svg, char_id(lig), x, y, lig2_w)

    for idx, lig in enumerate(LIGATURES_3):
        row = idx // lig3_cols
        col = idx % lig3_cols
        x = MARGIN_LEFT + col * (lig3_w + GUTTER)
        y = MARGIN_TOP + (lig3_row_offset + row) * ROW_STEP
        emit_cell_group(svg, char_id(lig), x, y, lig3_w)

    for ch, x, y_rel, w in arrow_layout:
        row = (y_rel - MARGIN_TOP) // ROW_STEP
        y = arrow_y_base + row * ROW_STEP
        g = emit_cell_group(svg, char_id(ch), x, y, w)
        if add_glyph_artwork(tt, ord(ch), x, y, w, g):
            filled += 1

    ET.indent(svg, space="  ")
    return ET.tostring(svg, encoding="unicode", xml_declaration=False), filled


for w in WEIGHTS:
    style = w["style"]
    src_file = SRC_MAP.get(style)
    if src_file is None:
        print(f"WARNING: no source font for style '{style}' -- skipping")
        continue

    src_path = os.path.join(SRC_DIR, src_file)
    if not os.path.exists(src_path):
        print(f"ERROR: source not found: {src_path}")
        continue

    tt = TTFont(src_path)
    svg_str, filled = build_prefilled_svg(tt)
    tt.close()

    out_path = os.path.join(OUT_DIR, w["svg"])
    with open(out_path, "w", encoding="utf-8") as f:
        f.write(svg_str)

    print(f"wrote {out_path} ({filled}/{len(CHARS)} glyphs filled)")

print("\nDone.")
