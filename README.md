# GGWP — GlyphSheet Generation Workflow Protocol

**Build fonts from SVG glyph sheets. No GUI font editor required.**

GUI font editors are drawing tools. But if you already have a better drawing environment — Affinity Designer, Figma, Inkscape — the only thing missing is the SVG-to-TTF conversion. GGWP fills that gap.

The glyph sheet is the single source of truth. The designer controls everything visually — outlines, metrics, spacing — and the build script reads it all from the SVG.

---

## The Chain

<img src="diagrams/readme-chain.svg" alt="The Chain">

1. **Define** font metadata, grid layout, and character set in a Python config
2. **Generate** blank SVG glyph sheets with reference lines and cell grid
3. **Draw** glyphs in any SVG editor
4. **Build** TTF fonts directly from the filled glyph sheets

---

## Quick Start

```bash
# 1. Copy and edit the default config
cp sheet_config.py my_font_config.py

# 2. Generate blank glyph sheets
python3 generate_sheet.py my_font_config.py

# 3. Open sheets in your SVG editor and draw glyphs

# 4. Build fonts
python3 build_fonts.py my_font_config.py
```

---

## Monospace vs Proportional

**Monospace** — all glyphs share the same advance width (= cell width). No sidebearing markers needed.

**Proportional** — each cell has cyan (LSB) and magenta (RSB) vertical markers. The designer positions them to control per-glyph spacing. Advance width = distance between markers.

---

## Reference Lines

Horizontal colored lines in each cell define vertical metrics:

| Color | Line | Purpose |
|---|---|---|
| Purple `#800080` | Capline | Top of capital letters |
| Green `#008000` | x-Height | Top of lowercase letters |
| Yellow `#808000` | Baseline | Where letters sit |
| Red `#FF0000` | Descender | Bottom of descenders (g, p, y) |
| Cyan `#00FFFF` | LSB | Left sidebearing (proportional only) |
| Magenta `#FF00FF` | RSB | Right sidebearing (proportional only) |

The build script reads these positions from the SVG. Adjust them in your editor to fine-tune metrics.

---

## Config

The config is a Python dict in a `.py` file. See [`sheet_config.py`](sheet_config.py) for the fully documented default.

```python
CONFIG = {
    "family_name": "My Font",
    "font_type": "monospace",       # or "proportional"
    "weights": [
        {"name": "Regular", "style": "Regular", "ps_suffix": "Regular",
         "weight_class": 400, "filename": "MyFont-Regular.svg"},
    ],
    "grid": {
        "columns": 10,
        "cell_width": 120,
        "cell_height": 223,
        "col_gap": 20,
        "row_gap": 20,
        ...
    },
    "reference_lines": {
        "capline":   {"y": 0,   "color": "#800080", "label": "Cap Height"},
        "xheight":   {"y": 40,  "color": "#008000", "label": "x-Height"},
        "baseline":  {"y": 145, "color": "#808000", "label": "Baseline"},
        "descender": {"y": 223, "color": "#FF0000", "label": "Descender"},
    },
    "codepoints": [chr(c) for c in range(0x21, 0x7F)],
}
```

---

## SVG Structure

The build script expects this element order per cell:

```xml
<text ...>A</text>              <!-- codepoint label -->
<line ... stroke="#800080"/>    <!-- capline -->
<line ... stroke="#008000"/>    <!-- x-height -->
<line ... stroke="#808000"/>    <!-- baseline -->
<line ... stroke="#FF0000"/>    <!-- descender -->
<line ... stroke="#00FFFF"/>    <!-- LSB marker (proportional only) -->
<line ... stroke="#FF00FF"/>    <!-- RSB marker (proportional only) -->
<g id="glyph-U+0041">          <!-- glyph paths (draw here) -->
  <path d="..." fill="white"/>
</g>
<rect ... fill="none"/>        <!-- cell border -->
```

The `<g>` must come before the cell border `<rect>`. The build script finds glyphs by scanning forward from the text label to the first `<g>`.

---

## Documentation

See [`GLYPHSHEET.md`](GLYPHSHEET.md) for comprehensive documentation including:

- Step-by-step wizard for creating new fonts
- Grid planning guide
- Metrics derivation formulas
- Monospace vs proportional details
- Troubleshooting guide
- Agent validation checklists

---

## Why This Exists

Font creation has been gatekept by tooling for decades.

The professional options — Glyphs ($300), RoboFont ($490), FontLab ($500) — are expensive, platform-locked, and ship with drawing tools worse than the vector editor you already own. The free option, FontForge, has a UI from 2003 and crashes regularly. The affordable option, Birdfont, is clunky and sells expiring licenses.

These tools fuse two completely separate concerns: **drawing glyphs** and **compiling font tables**. Drawing is a creative act. Compiling is engineering. There is no reason they need to live in the same application.

Meanwhile, the vector editor landscape has changed:

- **Affinity Designer** — now free (acquired by Canva), full-featured, sub-pixel precision
- **Inkscape** — always free, always capable
- **Figma** — free tier, browser-based, collaborative

The creative half of font making is now zero cost. GGWP is the other half. Also zero cost.

A font is just a collection of lookup tables — outlines, metrics, naming, encoding. Once you have vector paths in an SVG, building those tables is a deterministic, automatable process. No proprietary format. No magic. Just math and spec compliance.

You don't need to understand typography jargon to make a font. You don't need to know what "UPM" or "fsSelection bit 7" means. You need to draw letters that look right, place reference lines where they feel right, and run a script. The eyes work. The vocabulary is just labels.

Total cost of a professional font pipeline: `pip install fonttools`.

---

## Dependencies

- Python 3.8+
- [`fonttools`](https://github.com/fonttools/fonttools) (`pip install fonttools`)

---

## License

MIT — Use it, break it, fix it, ship it.

---
Rock 'n Roll!

**JRENG!**
---
conceived with [CAROL](https://github.com/jrengmusic/carol)
