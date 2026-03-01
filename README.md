# GGWP — GlyphSheet Generation Workflow Protocol

**Build fonts from SVG glyph sheets. No GUI font editor required.**

GUI font editors are drawing tools. But if you already have a better drawing environment — Affinity Designer, Figma, Inkscape — the only thing missing is the SVG-to-TTF conversion. GGWP fills that gap.

The glyph sheet is the single source of truth. The designer controls everything visually — outlines, metrics, spacing — and the build script reads it all from the SVG.

---

## The Chain

<svg id="my-svg" width="100%" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" class="flowchart" style="max-width: 1334.7px; background-color: transparent;" viewBox="0 0 1334.703125 70" role="graphics-document document" aria-roledescription="flowchart-v2"><style>#my-svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;fill:#333;}@keyframes edge-animation-frame{from{stroke-dashoffset:0;}}@keyframes dash{to{stroke-dashoffset:0;}}#my-svg .edge-animation-slow{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 50s linear infinite;stroke-linecap:round;}#my-svg .edge-animation-fast{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 20s linear infinite;stroke-linecap:round;}#my-svg .error-icon{fill:#552222;}#my-svg .error-text{fill:#552222;stroke:#552222;}#my-svg .edge-thickness-normal{stroke-width:1px;}#my-svg .edge-thickness-thick{stroke-width:3.5px;}#my-svg .edge-pattern-solid{stroke-dasharray:0;}#my-svg .edge-thickness-invisible{stroke-width:0;fill:none;}#my-svg .edge-pattern-dashed{stroke-dasharray:3;}#my-svg .edge-pattern-dotted{stroke-dasharray:2;}#my-svg .marker{fill:#333333;stroke:#333333;}#my-svg .marker.cross{stroke:#333333;}#my-svg svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;}#my-svg p{margin:0;}#my-svg .label{font-family:"trebuchet ms",verdana,arial,sans-serif;color:#333;}#my-svg .cluster-label text{fill:#333;}#my-svg .cluster-label span{color:#333;}#my-svg .cluster-label span p{background-color:transparent;}#my-svg .label text,#my-svg span{fill:#333;color:#333;}#my-svg .node rect,#my-svg .node circle,#my-svg .node ellipse,#my-svg .node polygon,#my-svg .node path{fill:#ECECFF;stroke:#9370DB;stroke-width:1px;}#my-svg .rough-node .label text,#my-svg .node .label text,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-anchor:middle;}#my-svg .node .katex path{fill:#000;stroke:#000;stroke-width:1px;}#my-svg .rough-node .label,#my-svg .node .label,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-align:center;}#my-svg .node.clickable{cursor:pointer;}#my-svg .root .anchor path{fill:#333333!important;stroke-width:0;stroke:#333333;}#my-svg .arrowheadPath{fill:#333333;}#my-svg .edgePath .path{stroke:#333333;stroke-width:2.0px;}#my-svg .flowchart-link{stroke:#333333;fill:none;}#my-svg .edgeLabel{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .edgeLabel p{background-color:rgba(232,232,232, 0.8);}#my-svg .edgeLabel rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .labelBkg{background-color:rgba(232, 232, 232, 0.5);}#my-svg .cluster rect{fill:#ffffde;stroke:#aaaa33;stroke-width:1px;}#my-svg .cluster text{fill:#333;}#my-svg .cluster span{color:#333;}#my-svg div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:12px;background:hsl(80, 100%, 96.2745098039%);border:1px solid #aaaa33;border-radius:2px;pointer-events:none;z-index:100;}#my-svg .flowchartTitleText{text-anchor:middle;font-size:18px;fill:#333;}#my-svg rect.text{fill:none;stroke-width:0;}#my-svg .icon-shape,#my-svg .image-shape{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .icon-shape p,#my-svg .image-shape p{background-color:rgba(232,232,232, 0.8);padding:2px;}#my-svg .icon-shape rect,#my-svg .image-shape rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .label-icon{display:inline-block;height:1em;overflow:visible;vertical-align:-0.125em;}#my-svg .node .label-icon path{fill:currentColor;stroke:revert;stroke-width:revert;}#my-svg :root{--mermaid-font-family:"trebuchet ms",verdana,arial,sans-serif;}</style><g><marker id="my-svg_flowchart-v2-pointEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 0 L 10 5 L 0 10 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-pointStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="4.5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 5 L 10 10 L 10 0 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="11" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="-1" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossEnd" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="12" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossStart" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="-1" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><g class="root"><g class="clusters"/><g class="edgePaths"><path d="M152.656,35L156.823,35C160.99,35,169.323,35,176.99,35C184.656,35,191.656,35,195.156,35L198.656,35" id="L_A_B_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M396.672,35L400.839,35C405.005,35,413.339,35,421.005,35C428.672,35,435.672,35,439.172,35L442.672,35" id="L_B_C_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M578.406,35L582.573,35C586.74,35,595.073,35,602.74,35C610.406,35,617.406,35,620.906,35L624.406,35" id="L_C_D_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M796.453,35L800.62,35C804.786,35,813.12,35,820.786,35C828.453,35,835.453,35,838.953,35L842.453,35" id="L_D_E_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M979.234,35L983.401,35C987.568,35,995.901,35,1003.568,35C1011.234,35,1018.234,35,1021.734,35L1025.234,35" id="L_E_F_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M1192.219,35L1196.385,35C1200.552,35,1208.885,35,1216.552,35C1224.219,35,1231.219,35,1234.719,35L1238.219,35" id="L_F_G_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/></g><g class="edgeLabels"><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g></g><g class="nodes"><g class="node default" id="flowchart-A-0" transform="translate(80.328125, 35)"><rect class="basic label-container" style="" x="-72.328125" y="-27" width="144.65625" height="54"/><g class="label" style="" transform="translate(-42.328125, -12)"><rect/><foreignObject width="84.65625" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>Config (.py)</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-B-1" transform="translate(299.6640625, 35)"><rect class="basic label-container" style="" x="-97.0078125" y="-27" width="194.015625" height="54"/><g class="label" style="" transform="translate(-67.0078125, -12)"><rect/><foreignObject width="134.015625" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>generate_sheet.py</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-C-3" transform="translate(512.5390625, 35)"><rect class="basic label-container" style="" x="-65.8671875" y="-27" width="131.734375" height="54"/><g class="label" style="" transform="translate(-35.8671875, -12)"><rect/><foreignObject width="71.734375" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>Blank SVG</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-D-5" transform="translate(712.4296875, 35)"><rect class="basic label-container" style="" x="-84.0234375" y="-27" width="168.046875" height="54"/><g class="label" style="" transform="translate(-54.0234375, -12)"><rect/><foreignObject width="108.046875" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>Designer draws</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-E-7" transform="translate(912.84375, 35)"><rect class="basic label-container" style="" x="-66.390625" y="-27" width="132.78125" height="54"/><g class="label" style="" transform="translate(-36.390625, -12)"><rect/><foreignObject width="72.78125" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>Filled SVG</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-F-9" transform="translate(1110.7265625, 35)"><rect class="basic label-container" style="" x="-81.4921875" y="-27" width="162.984375" height="54"/><g class="label" style="" transform="translate(-51.4921875, -12)"><rect/><foreignObject width="102.984375" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>build_fonts.py</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-G-11" transform="translate(1284.4609375, 35)"><rect class="basic label-container" style="" x="-42.2421875" y="-27" width="84.484375" height="54"/><g class="label" style="" transform="translate(-12.2421875, -12)"><rect/><foreignObject width="24.484375" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>.ttf</p></span></div></foreignObject></g></g></g></g></g></svg>

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
