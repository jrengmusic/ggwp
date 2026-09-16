#!/usr/bin/env python3
"""
Generate reference glyph sheets from Merriweather variable font.
Instances at wght 300/500/700 to match Display Book/Medium/Bold.
Scale derived from source font's sCapHeight, not sheet_config's CAP_H.
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
    CELL_W,
    CHARS,
    COLS,
    CAPLINE_Y,
    BASELINE_Y,
    CELL_STROKE,
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
)

ET.register_namespace("", "http://www.w3.org/2000/svg")

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
SRC_FONT = os.path.join(BASE_DIR, "Merriweather", "Merriweather-VariableFont_opsz,wdth,wght.ttf")
OUT_DIR = os.path.join(BASE_DIR, "sheets")
os.makedirs(OUT_DIR, exist_ok=True)

BASELINE_OFFSET = BASELINE_Y - MARGIN_TOP

INSTANCES = [
    {"wght": 300, "svg": "GlyphSheet-Merriweather-Light.svg"},
    {"wght": 500, "svg": "GlyphSheet-Merriweather-Medium.svg"},
    {"wght": 700, "svg": "GlyphSheet-Merriweather-Bold.svg"},
]


def ntos(v):
    s = f"{v:.2f}"
    if "." in s:
        s = s.rstrip("0").rstrip(".")
    return s


def char_id(ch):
    return "_" + "_".join(f"{ord(c):04X}" for c in ch)


def get_advance(glyph_set, cmap, ch, scale_inv):
    """Get cell width in SVG pixels from interpolated advance width."""
    cp = ord(ch) if len(ch) == 1 else None
    if cp is None or cp not in cmap:
        return CELL_W
    glyph_name = cmap[cp]
    if glyph_name not in glyph_set:
        return CELL_W
    advance = glyph_set[glyph_name].width
    return round(advance * scale_inv)


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


def add_glyph_artwork(glyph_set, cmap, codepoint, cell_x, cell_y, cell_w, g_elem, scale_inv):
    """Extract interpolated glyph outline, transform to SVG coords, add as <path>."""
    if codepoint not in cmap:
        return False

    glyph_name = cmap[codepoint]
    if glyph_name not in glyph_set:
        return False

    baseline_svg_y = cell_y + BASELINE_OFFSET
    xform = (scale_inv, 0, 0, -scale_inv, cell_x, baseline_svg_y)

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


def build_reference_svg(glyph_set, cmap, scale_inv):
    """Build SVG with guides and glyph artwork from interpolated variable font instance."""
    char_widths = {ch: get_advance(glyph_set, cmap, ch, scale_inv) for ch in CHARS}
    char_layout = compute_layout(CHARS, char_widths)
    char_rows = math.ceil(len(CHARS) / COLS)

    lig2_cols = COLS // 2
    lig2_rows = math.ceil(len(LIGATURES_2) / lig2_cols) if LIGATURES_2 else 0
    lig3_cols = COLS // 3
    lig3_rows = math.ceil(len(LIGATURES_3) / lig3_cols) if LIGATURES_3 else 0
    arrow_rows = math.ceil(len(ARROWS) / COLS) if ARROWS else 0

    total_rows = char_rows + lig2_rows + lig3_rows + arrow_rows

    max_x_extent = MARGIN_LEFT
    for _, x, _, w in char_layout:
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

    filled = 0

    for ch, x, y, w in char_layout:
        g = emit_cell_group(svg, char_id(ch), x, y, w)
        if add_glyph_artwork(glyph_set, cmap, ord(ch), x, y, w, g, scale_inv):
            filled += 1

    ET.indent(svg, space="  ")
    return ET.tostring(svg, encoding="unicode", xml_declaration=False), filled


tt = TTFont(SRC_FONT)
source_cap_h = tt["OS/2"].sCapHeight

for inst in INSTANCES:
    loc = {"wght": inst["wght"], "wdth": 100, "opsz": 18}
    gs = tt.getGlyphSet(location=loc)
    cmap = tt.getBestCmap()

    scale_inv = (BASELINE_Y - CAPLINE_Y) / source_cap_h

    svg_str, filled = build_reference_svg(gs, cmap, scale_inv)

    out_path = os.path.join(OUT_DIR, inst["svg"])
    with open(out_path, "w", encoding="utf-8") as f:
        f.write(svg_str)

    print(f"wrote {out_path} (wght={inst['wght']}, {filled}/{len(CHARS)} glyphs filled)")

tt.close()
print("\nDone.")
