# GGWP — GlyphSheet Generation Workflow Protocol

**Version:** 1.0
**Purpose:** A formalized pipeline for creating fonts from SVG glyph sheets.

---

## What Is This?

GGWP is a method for building TrueType fonts (.ttf) without a GUI font editor. The designer draws glyphs in any SVG editor (Affinity Designer, Inkscape, etc.) on a structured grid sheet. A build script reads the sheet and produces a valid TTF with correct metrics, naming, and encoding.

**The glyph sheet IS the single source of truth.** Everything the build script needs — outlines, metrics, spacing — is encoded visually in the SVG.

---

## The Chain

<svg id="my-svg" width="100%" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" class="flowchart" style="max-width: 276px; background-color: transparent;" viewBox="0 0 276 742" role="graphics-document document" aria-roledescription="flowchart-v2"><style>#my-svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;fill:#333;}@keyframes edge-animation-frame{from{stroke-dashoffset:0;}}@keyframes dash{to{stroke-dashoffset:0;}}#my-svg .edge-animation-slow{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 50s linear infinite;stroke-linecap:round;}#my-svg .edge-animation-fast{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 20s linear infinite;stroke-linecap:round;}#my-svg .error-icon{fill:#552222;}#my-svg .error-text{fill:#552222;stroke:#552222;}#my-svg .edge-thickness-normal{stroke-width:1px;}#my-svg .edge-thickness-thick{stroke-width:3.5px;}#my-svg .edge-pattern-solid{stroke-dasharray:0;}#my-svg .edge-thickness-invisible{stroke-width:0;fill:none;}#my-svg .edge-pattern-dashed{stroke-dasharray:3;}#my-svg .edge-pattern-dotted{stroke-dasharray:2;}#my-svg .marker{fill:#333333;stroke:#333333;}#my-svg .marker.cross{stroke:#333333;}#my-svg svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;}#my-svg p{margin:0;}#my-svg .label{font-family:"trebuchet ms",verdana,arial,sans-serif;color:#333;}#my-svg .cluster-label text{fill:#333;}#my-svg .cluster-label span{color:#333;}#my-svg .cluster-label span p{background-color:transparent;}#my-svg .label text,#my-svg span{fill:#333;color:#333;}#my-svg .node rect,#my-svg .node circle,#my-svg .node ellipse,#my-svg .node polygon,#my-svg .node path{fill:#ECECFF;stroke:#9370DB;stroke-width:1px;}#my-svg .rough-node .label text,#my-svg .node .label text,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-anchor:middle;}#my-svg .node .katex path{fill:#000;stroke:#000;stroke-width:1px;}#my-svg .rough-node .label,#my-svg .node .label,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-align:center;}#my-svg .node.clickable{cursor:pointer;}#my-svg .root .anchor path{fill:#333333!important;stroke-width:0;stroke:#333333;}#my-svg .arrowheadPath{fill:#333333;}#my-svg .edgePath .path{stroke:#333333;stroke-width:2.0px;}#my-svg .flowchart-link{stroke:#333333;fill:none;}#my-svg .edgeLabel{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .edgeLabel p{background-color:rgba(232,232,232, 0.8);}#my-svg .edgeLabel rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .labelBkg{background-color:rgba(232, 232, 232, 0.5);}#my-svg .cluster rect{fill:#ffffde;stroke:#aaaa33;stroke-width:1px;}#my-svg .cluster text{fill:#333;}#my-svg .cluster span{color:#333;}#my-svg div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:12px;background:hsl(80, 100%, 96.2745098039%);border:1px solid #aaaa33;border-radius:2px;pointer-events:none;z-index:100;}#my-svg .flowchartTitleText{text-anchor:middle;font-size:18px;fill:#333;}#my-svg rect.text{fill:none;stroke-width:0;}#my-svg .icon-shape,#my-svg .image-shape{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .icon-shape p,#my-svg .image-shape p{background-color:rgba(232,232,232, 0.8);padding:2px;}#my-svg .icon-shape rect,#my-svg .image-shape rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .label-icon{display:inline-block;height:1em;overflow:visible;vertical-align:-0.125em;}#my-svg .node .label-icon path{fill:currentColor;stroke:revert;stroke-width:revert;}#my-svg :root{--mermaid-font-family:"trebuchet ms",verdana,arial,sans-serif;}</style><g><marker id="my-svg_flowchart-v2-pointEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 0 L 10 5 L 0 10 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-pointStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="4.5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 5 L 10 10 L 10 0 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="11" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="-1" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossEnd" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="12" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossStart" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="-1" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><g class="root"><g class="clusters"/><g class="edgePaths"><path d="M138,158L138,162.167C138,166.333,138,174.667,138,182.333C138,190,138,197,138,200.5L138,204" id="L_A_B_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M138,334L138,338.167C138,342.333,138,350.667,138,358.333C138,366,138,373,138,376.5L138,380" id="L_B_C_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M138,510L138,514.167C138,518.333,138,526.667,138,534.333C138,542,138,549,138,552.5L138,556" id="L_C_D_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/></g><g class="edgeLabels"><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g></g><g class="nodes"><g class="node default" id="flowchart-A-0" transform="translate(138, 83)"><rect class="basic label-container" style="" x="-130" y="-75" width="260" height="150"/><g class="label" style="" transform="translate(-100, -60)"><rect/><foreignObject width="200" height="120"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel">1. CONFIG&#10;Define font metadata, grid, codepoints, weights&#10;(Python dict in a .py file)</span></div></foreignObject></g></g><g class="node default" id="flowchart-B-1" transform="translate(138, 271)"><rect class="basic label-container" style="" x="-130" y="-63" width="260" height="126"/><g class="label" style="" transform="translate(-100, -48)"><rect/><foreignObject width="200" height="96"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel">2. GENERATE&#10;Run generate_sheet.py to emit blank SVG sheets&#10;(one sheet per weight)</span></div></foreignObject></g></g><g class="node default" id="flowchart-C-2" transform="translate(138, 447)"><rect class="basic label-container" style="" x="-130" y="-63" width="260" height="126"/><g class="label" style="" transform="translate(-100, -48)"><rect/><foreignObject width="200" height="96"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel">3. DRAW&#10;Designer fills glyphs into cells using any SVG editor&#10;(Affinity Designer, Inkscape, etc.)</span></div></foreignObject></g></g><g class="node default" id="flowchart-D-3" transform="translate(138, 647)"><rect class="basic label-container" style="" x="-130" y="-87" width="260" height="174"/><g class="label" style="" transform="translate(-100, -72)"><rect/><foreignObject width="200" height="144"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel">4. BUILD&#10;Run build_fonts.py to parse filled sheets into TTFs&#10;(reads everything from the SVG — no manual metric entry)</span></div></foreignObject></g></g></g></g></g></svg>

Each step is independent. You can regenerate sheets without losing drawn glyphs (just don't overwrite). You can rebuild fonts any time the SVG changes.

---

## For Agents: Step-by-Step Wizard

When guiding a human through creating a new font, follow these steps in order. Ask questions at each step before proceeding.

### Step 1: Gather Requirements

Ask the human:

1. **Font name?** (family name, e.g., "My Sans")
2. **Monospace or proportional?**
3. **How many weights?** (e.g., Light, Regular, Bold)
4. **What characters to include?** (ASCII only? Latin Extended? Custom set?)
5. **Target use case?** (terminal, display, body text — affects grid sizing)
6. **Metadata:** copyright, designer name, vendor ID (4 chars), URL

### Step 2: Plan Grid Dimensions

Based on the font type and character set:

| Decision | Monospace | Proportional |
|---|---|---|
| cell_width | Width of widest glyph (e.g., M, W) | Width of widest glyph + generous margin |
| cell_height | Ascender-to-descender + overshoot room | Same |
| col_gap | 10-20px (visual separation only) | Same |
| Markers needed | None (cell_width = advance) | LSB (cyan) + RSB (magenta) per cell |

**Rule of thumb:** For a typical Latin text font at ~120px cell width, use:
- `cell_width`: 120px (monospace) or 160px (proportional, room for W/M)
- `cell_height`: 200-250px (depends on ascender/descender ratio)
- `col_gap`: 20px
- `row_gap`: 20px
- `label_height`: 15px
- `columns`: 10

### Step 3: Set Reference Line Positions

Reference lines define the font's vertical metrics. They are horizontal colored lines in each cell:

| Line | Color | Meaning | Typical Position |
|---|---|---|---|
| **Capline** | Purple `#800080` | Top of capital letters (H, E, B) | ~0px from cell top |
| **x-Height** | Green `#008000` | Top of lowercase letters (x, o, a) | ~40% down from capline |
| **Baseline** | Yellow `#808000` | Where letters sit | ~65% down from cell top |
| **Descender** | Red `#FF0000` | Bottom of descenders (g, p, y) | Cell bottom or near it |

**These are initial positions.** The designer can adjust them in the SVG editor. The build script reads the actual positions from the SVG, not from the config.

**How the build script uses them:**
- `scale = capH / (baseline_y - capline_y)` where `capH` is the target cap height in font units (739)
- `ascender = (baseline_y - cell_top) * scale` (cell_top = margin_top + label_height)
- `descender = -(descender_y - baseline_y) * scale` (negative value)
- `UPM = ascender + abs(descender)`
- Or use `metrics_override` per weight to bypass auto-derivation entirely

### Step 4: Write the Config

Copy `sheet_config.py` and modify. Every field is documented in the file.

```python
CONFIG = {
    "family_name": "My Sans",
    "font_type": "proportional",    # or "monospace"
    "version": "1.0",
    "copyright": "...",
    "vendor": "ABCD",
    "designer": "Your Name",
    "description": "A clean sans-serif.",
    "url": "https://example.com",

    "weights": [
        {"name": "Regular", "style": "Regular", "ps_suffix": "Regular",
         "weight_class": 400, "filename": "MySans-Regular.svg"},
        {"name": "Bold", "style": "Bold", "ps_suffix": "Bold",
         "weight_class": 700, "filename": "MySans-Bold.svg"},
    ],

    "grid": {
        "columns": 10,
        "cell_width": 160,
        "cell_height": 240,
        "col_gap": 20,
        "row_gap": 20,
        "label_height": 15,
        "margin_left": 40,
        "margin_top": 60,
        "margin_right": 20,
        "margin_bottom": 40,
    },

    "reference_lines": {
        "capline":   {"y": 10,  "color": "#800080", "label": "Cap Height"},
        "xheight":   {"y": 80,  "color": "#008000", "label": "x-Height"},
        "baseline":  {"y": 180, "color": "#808000", "label": "Baseline"},
        "descender": {"y": 230, "color": "#FF0000", "label": "Descender"},
    },

    "sidebearing_markers": {
        "lsb_color": "#00FFFF",
        "rsb_color": "#FF00FF",
    },

    "codepoints": [chr(c) for c in range(0x21, 0x7F)],  # ASCII printable

    "output": {
        "sheets_dir": "sheets",
        "fonts_dir": "fonts",
    },
}
```

### Step 5: Generate Blank Sheets

```bash
python3 generate_sheet.py my_config.py          # all weights
python3 generate_sheet.py my_config.py Regular   # single weight only
```

Run from the directory where your config and the ggwp scripts live.

This creates one SVG per weight in the `sheets_dir` directory. Each SVG contains:
- Dark grey background
- Grid of cells with codepoint labels
- Reference lines (capline, x-height, baseline, descender) in each cell
- LSB/RSB markers in each cell (proportional fonts only)
- Empty `<g>` elements where glyphs will be drawn
- Cell border rectangles for visual alignment

### Step 6: Draw Glyphs

Open each SVG in your editor (Affinity Designer recommended). For each cell:

1. **Draw the glyph** inside the cell boundaries
2. **Align to reference lines:**
   - Capital letters touch the purple capline and sit on the yellow baseline
   - Lowercase letters touch the green x-height and sit on the yellow baseline
   - Descenders (g, p, y) extend down to the red descender line
3. **For proportional fonts:** adjust the cyan (LSB) and magenta (RSB) vertical markers to set the glyph's sidebearings

**Rules for drawing:**
- All glyph paths must be inside the `<g>` element for that cell
- Use filled paths (not strokes) — the build script reads path outlines
- The `<g>` element must come BEFORE the cell border `<rect>` in SVG source order
- Paths from other cells must not leak into a cell's `<g>` element

### Step 7: Build Fonts

```bash
python3 build_fonts.py my_config.py          # all weights
python3 build_fonts.py my_config.py Bold     # single weight only
```

This reads each filled SVG sheet and produces a TTF in `fonts_dir`. The build script:
- Parses reference line positions from the SVG (not from config)
- Extracts glyph paths from each cell's `<g>` element
- Transforms SVG coordinates to font units (Y-flip, scaling)
- Converts cubic beziers to quadratic (TrueType requirement)
- Sets per-glyph advance width and LSB
- Writes all font tables (name, OS/2, cmap, glyf, hmtx, etc.)

---

## SVG Sheet Anatomy

<svg id="my-svg" width="100%" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" style="max-width: 448.578px; background-color: transparent;" viewBox="-5 -452.5 448.578125 457.5" role="graphics-document document" aria-roledescription="block"><style>#my-svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;fill:#333;}@keyframes edge-animation-frame{from{stroke-dashoffset:0;}}@keyframes dash{to{stroke-dashoffset:0;}}#my-svg .edge-animation-slow{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 50s linear infinite;stroke-linecap:round;}#my-svg .edge-animation-fast{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 20s linear infinite;stroke-linecap:round;}#my-svg .error-icon{fill:#552222;}#my-svg .error-text{fill:#552222;stroke:#552222;}#my-svg .edge-thickness-normal{stroke-width:1px;}#my-svg .edge-thickness-thick{stroke-width:3.5px;}#my-svg .edge-pattern-solid{stroke-dasharray:0;}#my-svg .edge-thickness-invisible{stroke-width:0;fill:none;}#my-svg .edge-pattern-dashed{stroke-dasharray:3;}#my-svg .edge-pattern-dotted{stroke-dasharray:2;}#my-svg .marker{fill:#333333;stroke:#333333;}#my-svg .marker.cross{stroke:#333333;}#my-svg svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;}#my-svg p{margin:0;}#my-svg .label{font-family:"trebuchet ms",verdana,arial,sans-serif;color:#333;}#my-svg .cluster-label text{fill:#333;}#my-svg .cluster-label span,#my-svg p{color:#333;}#my-svg .label text,#my-svg span,#my-svg p{fill:#333;color:#333;}#my-svg .node rect,#my-svg .node circle,#my-svg .node ellipse,#my-svg .node polygon,#my-svg .node path{fill:#ECECFF;stroke:#9370DB;stroke-width:1px;}#my-svg .flowchart-label text{text-anchor:middle;}#my-svg .node .label{text-align:center;}#my-svg .node.clickable{cursor:pointer;}#my-svg .arrowheadPath{fill:#333333;}#my-svg .edgePath .path{stroke:#333333;stroke-width:2.0px;}#my-svg .flowchart-link{stroke:#333333;fill:none;}#my-svg .edgeLabel{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .edgeLabel rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .labelBkg{background-color:rgba(232, 232, 232, 0.5);}#my-svg .node .cluster{fill:rgba(255, 255, 222, 0.5);stroke:rgba(170, 170, 51, 0.2);stroke-width:1px;}#my-svg .cluster text{fill:#333;}#my-svg .cluster span,#my-svg p{color:#333;}#my-svg div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:12px;background:hsl(80, 100%, 96.2745098039%);border:1px solid #aaaa33;border-radius:2px;pointer-events:none;z-index:100;}#my-svg .flowchartTitleText{text-anchor:middle;font-size:18px;fill:#333;}#my-svg .label-icon{display:inline-block;height:1em;overflow:visible;vertical-align:-0.125em;}#my-svg .node .label-icon path{fill:currentColor;stroke:revert;stroke-width:revert;}#my-svg :root{--mermaid-font-family:"trebuchet ms",verdana,arial,sans-serif;}</style><g/><marker id="my-svg_block-pointEnd" class="marker block" viewBox="0 0 10 10" refX="6" refY="5" markerUnits="userSpaceOnUse" markerWidth="12" markerHeight="12" orient="auto"><path d="M 0 0 L 10 5 L 0 10 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_block-pointStart" class="marker block" viewBox="0 0 10 10" refX="4.5" refY="5" markerUnits="userSpaceOnUse" markerWidth="12" markerHeight="12" orient="auto"><path d="M 0 5 L 10 10 L 10 0 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_block-circleEnd" class="marker block" viewBox="0 0 10 10" refX="11" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_block-circleStart" class="marker block" viewBox="0 0 10 10" refX="-1" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_block-crossEnd" class="marker cross block" viewBox="0 0 11 11" refX="12" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_block-crossStart" class="marker cross block" viewBox="0 0 11 11" refX="-1" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><g class="block"><g class="node default default flowchart-label" id="sheet" transform="translate(70.4296875, -387)"><rect class="basic label-container" style="" rx="0" ry="0" x="-70.4296875" y="-60.5" width="140.859375" height="121"/><g class="label" style="" transform="translate(-36.484375, -9.5)"><rect/><foreignObject width="72.96875" height="19"><div xmlns="http://www.w3.org/1999/xhtml" style="display: inline-block; white-space: nowrap;"><span class="nodeLabel">SVG Sheet</span></div></foreignObject></g></g><g class="node default default flowchart-label" id="row1" transform="translate(219.2890625, -258)"><rect class="basic cluster composite label-container" style="" rx="0" ry="0" x="-219.2890625" y="-60.5" width="438.578125" height="121"/><g class="label" style="" transform="translate(0, 0)"><rect/><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" style="display: inline-block; white-space: nowrap;"><span class="nodeLabel"></span></div></foreignObject></g></g><g class="node default default flowchart-label" id="cellA" transform="translate(75.76302083333333, -258)"><rect class="basic label-container" style="" rx="0" ry="0" x="-67.76302083333333" y="-52.5" width="135.52604166666666" height="105"/><g class="label" style="" transform="translate(-50.4296875, -48.5)"><rect/><foreignObject width="100.859375" height="97"><div xmlns="http://www.w3.org/1999/xhtml" style="display: inline-block; white-space: nowrap;"><span class="nodeLabel">A<br/>&#x2550;&#x2550; capline<br/>&#x2500;&#x2500; x-height<br/>&#x2550;&#x2550; baseline<br/>&#x2500;&#x2500; descender</span></div></foreignObject></g></g><g class="node default default flowchart-label" id="cellB" transform="translate(219.2890625, -258)"><rect class="basic label-container" style="" rx="0" ry="0" x="-67.76302083333333" y="-52.5" width="135.52604166666666" height="105"/><g class="label" style="" transform="translate(-50.4296875, -48.5)"><rect/><foreignObject width="100.859375" height="97"><div xmlns="http://www.w3.org/1999/xhtml" style="display: inline-block; white-space: nowrap;"><span class="nodeLabel">B<br/>&#x2550;&#x2550; capline<br/>&#x2500;&#x2500; x-height<br/>&#x2550;&#x2550; baseline<br/>&#x2500;&#x2500; descender</span></div></foreignObject></g></g><g class="node default default flowchart-label" id="more" transform="translate(362.81510416666663, -258)"><rect class="basic label-container" style="" rx="0" ry="0" x="-67.76302083333333" y="-52.5" width="135.52604166666666" height="105"/><g class="label" style="" transform="translate(-8.8125, -9.5)"><rect/><foreignObject width="17.625" height="19"><div xmlns="http://www.w3.org/1999/xhtml" style="display: inline-block; white-space: nowrap;"><span class="nodeLabel">...</span></div></foreignObject></g></g><g class="node default default flowchart-label" id="row2" transform="translate(219.2890625, -129)"><rect class="basic cluster composite label-container" style="" rx="0" ry="0" x="-219.2890625" y="-60.5" width="438.578125" height="121"/><g class="label" style="" transform="translate(0, 0)"><rect/><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" style="display: inline-block; white-space: nowrap;"><span class="nodeLabel"></span></div></foreignObject></g></g><g class="node default default flowchart-label" id="cellK" transform="translate(75.76302083333333, -129)"><rect class="basic label-container" style="" rx="0" ry="0" x="-67.76302083333333" y="-52.5" width="135.52604166666666" height="105"/><g class="label" style="" transform="translate(-8.8125, -19)"><rect/><foreignObject width="17.625" height="38"><div xmlns="http://www.w3.org/1999/xhtml" style="display: inline-block; white-space: nowrap;"><span class="nodeLabel">K<br/>...</span></div></foreignObject></g></g><g class="node default default flowchart-label" id="cellL" transform="translate(219.2890625, -129)"><rect class="basic label-container" style="" rx="0" ry="0" x="-67.76302083333333" y="-52.5" width="135.52604166666666" height="105"/><g class="label" style="" transform="translate(-8.8125, -19)"><rect/><foreignObject width="17.625" height="38"><div xmlns="http://www.w3.org/1999/xhtml" style="display: inline-block; white-space: nowrap;"><span class="nodeLabel">L<br/>...</span></div></foreignObject></g></g><g class="node default default flowchart-label" id="more2" transform="translate(362.81510416666663, -129)"><rect class="basic label-container" style="" rx="0" ry="0" x="-67.76302083333333" y="-52.5" width="135.52604166666666" height="105"/><g class="label" style="" transform="translate(-8.8125, -9.5)"><rect/><foreignObject width="17.625" height="19"><div xmlns="http://www.w3.org/1999/xhtml" style="display: inline-block; white-space: nowrap;"><span class="nodeLabel">...</span></div></foreignObject></g></g></g></svg>

For proportional fonts, each cell also has:

<svg id="my-svg" width="100%" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" class="flowchart" style="max-width: 1222.12px; background-color: transparent;" viewBox="0 0 1222.125 143" role="graphics-document document" aria-roledescription="flowchart-v2"><style>#my-svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;fill:#333;}@keyframes edge-animation-frame{from{stroke-dashoffset:0;}}@keyframes dash{to{stroke-dashoffset:0;}}#my-svg .edge-animation-slow{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 50s linear infinite;stroke-linecap:round;}#my-svg .edge-animation-fast{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 20s linear infinite;stroke-linecap:round;}#my-svg .error-icon{fill:#552222;}#my-svg .error-text{fill:#552222;stroke:#552222;}#my-svg .edge-thickness-normal{stroke-width:1px;}#my-svg .edge-thickness-thick{stroke-width:3.5px;}#my-svg .edge-pattern-solid{stroke-dasharray:0;}#my-svg .edge-thickness-invisible{stroke-width:0;fill:none;}#my-svg .edge-pattern-dashed{stroke-dasharray:3;}#my-svg .edge-pattern-dotted{stroke-dasharray:2;}#my-svg .marker{fill:#333333;stroke:#333333;}#my-svg .marker.cross{stroke:#333333;}#my-svg svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;}#my-svg p{margin:0;}#my-svg .label{font-family:"trebuchet ms",verdana,arial,sans-serif;color:#333;}#my-svg .cluster-label text{fill:#333;}#my-svg .cluster-label span{color:#333;}#my-svg .cluster-label span p{background-color:transparent;}#my-svg .label text,#my-svg span{fill:#333;color:#333;}#my-svg .node rect,#my-svg .node circle,#my-svg .node ellipse,#my-svg .node polygon,#my-svg .node path{fill:#ECECFF;stroke:#9370DB;stroke-width:1px;}#my-svg .rough-node .label text,#my-svg .node .label text,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-anchor:middle;}#my-svg .node .katex path{fill:#000;stroke:#000;stroke-width:1px;}#my-svg .rough-node .label,#my-svg .node .label,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-align:center;}#my-svg .node.clickable{cursor:pointer;}#my-svg .root .anchor path{fill:#333333!important;stroke-width:0;stroke:#333333;}#my-svg .arrowheadPath{fill:#333333;}#my-svg .edgePath .path{stroke:#333333;stroke-width:2.0px;}#my-svg .flowchart-link{stroke:#333333;fill:none;}#my-svg .edgeLabel{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .edgeLabel p{background-color:rgba(232,232,232, 0.8);}#my-svg .edgeLabel rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .labelBkg{background-color:rgba(232, 232, 232, 0.5);}#my-svg .cluster rect{fill:#ffffde;stroke:#aaaa33;stroke-width:1px;}#my-svg .cluster text{fill:#333;}#my-svg .cluster span{color:#333;}#my-svg div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:12px;background:hsl(80, 100%, 96.2745098039%);border:1px solid #aaaa33;border-radius:2px;pointer-events:none;z-index:100;}#my-svg .flowchartTitleText{text-anchor:middle;font-size:18px;fill:#333;}#my-svg rect.text{fill:none;stroke-width:0;}#my-svg .icon-shape,#my-svg .image-shape{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .icon-shape p,#my-svg .image-shape p{background-color:rgba(232,232,232, 0.8);padding:2px;}#my-svg .icon-shape rect,#my-svg .image-shape rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .label-icon{display:inline-block;height:1em;overflow:visible;vertical-align:-0.125em;}#my-svg .node .label-icon path{fill:currentColor;stroke:revert;stroke-width:revert;}#my-svg :root{--mermaid-font-family:"trebuchet ms",verdana,arial,sans-serif;}</style><g><marker id="my-svg_flowchart-v2-pointEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 0 L 10 5 L 0 10 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-pointStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="4.5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 5 L 10 10 L 10 0 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="11" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="-1" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossEnd" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="12" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossStart" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="-1" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><g class="root"><g class="clusters"/><g class="edgePaths"><path d="M268,90.032L272.167,91.027C276.333,92.022,284.667,94.011,293,95.005C301.333,96,309.667,96,313.833,96L318,96" id="L_LSB_G_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M460,96L469.755,96C479.51,96,499.021,96,518.531,96C538.042,96,557.552,96,567.307,96L577.063,96" id="L_G_RSB_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M268,27.968L272.167,26.973C276.333,25.978,284.667,23.989,304.833,22.995C325,22,357,22,394.589,22C432.177,22,475.354,22,528.365,22C581.375,22,644.219,22,707.063,22C769.906,22,832.75,22,873.273,23.786C913.796,25.572,931.998,29.144,941.099,30.931L950.2,32.717" id="L_LSB_AW_0" class="edge-thickness-normal edge-pattern-dotted edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M837.063,96L846.818,96C856.573,96,876.083,96,894.94,94.214C913.796,92.428,931.998,88.856,941.099,87.069L950.2,85.283" id="L_RSB_AW_0" class="edge-thickness-normal edge-pattern-dotted edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/></g><g class="edgeLabels"><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel" transform="translate(518.53125, 22)"><g class="label" transform="translate(-33.53125, -12)"><foreignObject width="67.0625" height="24"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"><p>measures</p></span></div></foreignObject></g></g><g class="edgeLabel" transform="translate(895.59375, 96)"><g class="label" transform="translate(-33.53125, -12)"><foreignObject width="67.0625" height="24"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"><p>measures</p></span></div></foreignObject></g></g></g><g class="nodes"><g class="node default" id="flowchart-LSB-0" transform="translate(138, 59)"><rect class="basic label-container" style="" x="-130" y="-39" width="260" height="78"/><g class="label" style="" transform="translate(-100, -24)"><rect/><foreignObject width="200" height="48"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>LSB marker&#10;(cyan vertical line)</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-G-1" transform="translate(389, 96)"><rect class="basic label-container" style="" x="-71" y="-27" width="142" height="54"/><g class="label" style="" transform="translate(-41, -12)"><rect/><foreignObject width="82" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>glyph paths</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-RSB-2" transform="translate(707.0625, 96)"><rect class="basic label-container" style="" x="-130" y="-39" width="260" height="78"/><g class="label" style="" transform="translate(-100, -24)"><rect/><foreignObject width="200" height="48"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>RSB marker&#10;(magenta vertical line)</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-AW-6" transform="translate(1084.125, 59)"><rect class="basic label-container" style="" x="-130" y="-51" width="260" height="102"/><g class="label" style="" transform="translate(-100, -36)"><rect/><foreignObject width="200" height="72"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>advance_width = RSB_x &#x2212; LSB_x&#10;(SVG px &#x2192; font units via scale)</p></span></div></foreignObject></g></g></g></g></g></svg>

---

## SVG Structure (Source Order)

The build script relies on a specific element ordering within the SVG. This is the order `generate_sheet.py` produces, and the order `build_fonts.py` expects:

```xml
<!-- For each cell: -->

<!-- 1. Codepoint label (text element with font-size:7px) -->
<text x="42" y="72" style="font-size:7px" ...>A</text>

<!-- 2. Reference lines (horizontal lines with specific colors) -->
<line x1="40" y1="75" x2="160" y2="75" stroke="#800080" .../>
<line x1="40" y1="115" x2="160" y2="115" stroke="#008000" .../>
<line x1="40" y1="220" x2="160" y2="220" stroke="#808000" .../>
<line x1="40" y1="298" x2="160" y2="298" stroke="#FF0000" .../>

<!-- 3. LSB/RSB markers (proportional only) -->
<line x1="52" y1="75" x2="52" y2="298" stroke="#00FFFF" .../>
<line x1="148" y1="75" x2="148" y2="298" stroke="#FF00FF" .../>

<!-- 4. Glyph paths (inside <g>, designer draws here) -->
<g id="glyph-U+0041">
  <path d="M 60 220 L 100 75 L 140 220 ..." fill="white"/>
</g>

<!-- 5. Cell border rectangle (AFTER the <g>) -->
<rect x="40" y="75" width="120" height="223" fill="none" stroke="#333355"/>
```

**Critical:** The `<g>` element MUST come BEFORE the cell border `<rect>`. The build script finds glyphs by scanning forward from the text label to the first `<g>` element.

---

## Reference Line Colors

| Color | Hex | Purpose | Build Script Reads As |
|---|---|---|---|
| Purple | `#800080` | Cap height line | `capline_y` — top of H, E, B |
| Green | `#008000` | x-height line | `xheight_y` — top of x, o, a |
| Yellow | `#808000` | Baseline | `baseline_y` — where letters sit |
| Red | `#FF0000` | Descender line | `descender_y` — bottom of g, p, y |
| Cyan | `#00FFFF` | LSB marker | Left sidebearing boundary |
| Magenta | `#FF00FF` | RSB marker | Right sidebearing boundary |

---

## Monospace vs Proportional

### Monospace

- `font_type = "monospace"`
- All glyphs have the same advance width = `cell_width * scale`
- No LSB/RSB markers needed
- `isFixedPitch = 1` in the `post` table
- Panose proportion = 9 (Monospaced)
- Glyphs should be visually centered within the cell

### Proportional

- `font_type = "proportional"`
- Each glyph has its own advance width defined by LSB/RSB markers
- `advance_width = (RSB_x - LSB_x) * scale`
- `LSB = (glyph_xMin - LSB_marker_x) * scale` (can be 0 or negative)
- `isFixedPitch = 0` in the `post` table
- Panose proportion = 0 (Any) or 2 (Old Style)
- Designer controls spacing visually by moving the markers

---

## Metrics Derivation

The build script supports two modes: **auto-derivation** from SVG geometry (default), or **explicit override** via `metrics_override` in the weight config.

### Auto-derivation (default)

<svg id="my-svg" width="100%" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" class="flowchart" style="max-width: 1516px; background-color: transparent;" viewBox="0 0 1516 550" role="graphics-document document" aria-roledescription="flowchart-v2"><style>#my-svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;fill:#333;}@keyframes edge-animation-frame{from{stroke-dashoffset:0;}}@keyframes dash{to{stroke-dashoffset:0;}}#my-svg .edge-animation-slow{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 50s linear infinite;stroke-linecap:round;}#my-svg .edge-animation-fast{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 20s linear infinite;stroke-linecap:round;}#my-svg .error-icon{fill:#552222;}#my-svg .error-text{fill:#552222;stroke:#552222;}#my-svg .edge-thickness-normal{stroke-width:1px;}#my-svg .edge-thickness-thick{stroke-width:3.5px;}#my-svg .edge-pattern-solid{stroke-dasharray:0;}#my-svg .edge-thickness-invisible{stroke-width:0;fill:none;}#my-svg .edge-pattern-dashed{stroke-dasharray:3;}#my-svg .edge-pattern-dotted{stroke-dasharray:2;}#my-svg .marker{fill:#333333;stroke:#333333;}#my-svg .marker.cross{stroke:#333333;}#my-svg svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;}#my-svg p{margin:0;}#my-svg .label{font-family:"trebuchet ms",verdana,arial,sans-serif;color:#333;}#my-svg .cluster-label text{fill:#333;}#my-svg .cluster-label span{color:#333;}#my-svg .cluster-label span p{background-color:transparent;}#my-svg .label text,#my-svg span{fill:#333;color:#333;}#my-svg .node rect,#my-svg .node circle,#my-svg .node ellipse,#my-svg .node polygon,#my-svg .node path{fill:#ECECFF;stroke:#9370DB;stroke-width:1px;}#my-svg .rough-node .label text,#my-svg .node .label text,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-anchor:middle;}#my-svg .node .katex path{fill:#000;stroke:#000;stroke-width:1px;}#my-svg .rough-node .label,#my-svg .node .label,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-align:center;}#my-svg .node.clickable{cursor:pointer;}#my-svg .root .anchor path{fill:#333333!important;stroke-width:0;stroke:#333333;}#my-svg .arrowheadPath{fill:#333333;}#my-svg .edgePath .path{stroke:#333333;stroke-width:2.0px;}#my-svg .flowchart-link{stroke:#333333;fill:none;}#my-svg .edgeLabel{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .edgeLabel p{background-color:rgba(232,232,232, 0.8);}#my-svg .edgeLabel rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .labelBkg{background-color:rgba(232, 232, 232, 0.5);}#my-svg .cluster rect{fill:#ffffde;stroke:#aaaa33;stroke-width:1px;}#my-svg .cluster text{fill:#333;}#my-svg .cluster span{color:#333;}#my-svg div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:12px;background:hsl(80, 100%, 96.2745098039%);border:1px solid #aaaa33;border-radius:2px;pointer-events:none;z-index:100;}#my-svg .flowchartTitleText{text-anchor:middle;font-size:18px;fill:#333;}#my-svg rect.text{fill:none;stroke-width:0;}#my-svg .icon-shape,#my-svg .image-shape{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .icon-shape p,#my-svg .image-shape p{background-color:rgba(232,232,232, 0.8);padding:2px;}#my-svg .icon-shape rect,#my-svg .image-shape rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .label-icon{display:inline-block;height:1em;overflow:visible;vertical-align:-0.125em;}#my-svg .node .label-icon path{fill:currentColor;stroke:revert;stroke-width:revert;}#my-svg :root{--mermaid-font-family:"trebuchet ms",verdana,arial,sans-serif;}</style><g><marker id="my-svg_flowchart-v2-pointEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 0 L 10 5 L 0 10 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-pointStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="4.5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 5 L 10 10 L 10 0 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="11" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="-1" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossEnd" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="12" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossStart" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="-1" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><g class="root"><g class="clusters"/><g class="edgePaths"><path d="M758,110L758,114.167C758,118.333,758,126.667,758,134.333C758,142,758,149,758,152.5L758,156" id="L_SVG_SCALE_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M628,226.935L546.333,236.946C464.667,246.957,301.333,266.978,219.667,282.489C138,298,138,309,138,314.5L138,320" id="L_SCALE_ASC_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M628,242.871L598,250.226C568,257.581,508,272.29,478,283.145C448,294,448,301,448,304.5L448,308" id="L_SCALE_DESC_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M758,262L758,266.167C758,270.333,758,278.667,758,288.333C758,298,758,309,758,314.5L758,320" id="L_SCALE_XH_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M138,402L138,408.167C138,414.333,138,426.667,147.475,436.746C156.95,446.824,175.9,454.649,185.375,458.561L194.85,462.473" id="L_ASC_UPM_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M448,414L448,418.167C448,422.333,448,430.667,438.525,438.746C429.05,446.824,410.1,454.649,400.625,458.561L391.15,462.473" id="L_DESC_UPM_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M888,242.871L918,250.226C948,257.581,1008,272.29,1038,285.145C1068,298,1068,309,1068,314.5L1068,320" id="L_SCALE_AW_MONO_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M888,226.935L969.667,236.946C1051.333,246.957,1214.667,266.978,1296.333,282.489C1378,298,1378,309,1378,314.5L1378,320" id="L_SCALE_AW_PROP_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/></g><g class="edgeLabels"><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g></g><g class="nodes"><g class="node default" id="flowchart-SVG-0" transform="translate(758, 59)"><rect class="basic label-container" style="" x="-130" y="-51" width="260" height="102"/><g class="label" style="" transform="translate(-100, -36)"><rect/><foreignObject width="200" height="72"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>SVG row 0&#10;capline_y &#xB7; xheight_y &#xB7; baseline_y &#xB7; descender_y</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-SCALE-1" transform="translate(758, 211)"><rect class="basic label-container" style="" x="-130" y="-51" width="260" height="102"/><g class="label" style="" transform="translate(-100, -36)"><rect/><foreignObject width="200" height="72"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>scale = capH &#xF7; (baseline_y &#x2212; capline_y)&#10;capH = 739 font units</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-ASC-2" transform="translate(138, 363)"><rect class="basic label-container" style="" x="-130" y="-39" width="260" height="78"/><g class="label" style="" transform="translate(-100, -24)"><rect/><foreignObject width="200" height="48"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>ascender = (baseline_y &#x2212; cell_top_y) &#xD7; scale</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-DESC-3" transform="translate(448, 363)"><rect class="basic label-container" style="" x="-130" y="-51" width="260" height="102"/><g class="label" style="" transform="translate(-100, -36)"><rect/><foreignObject width="200" height="72"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>descender = &#x2212;(descender_y &#x2212; baseline_y) &#xD7; scale</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-XH-4" transform="translate(758, 363)"><rect class="basic label-container" style="" x="-130" y="-39" width="260" height="78"/><g class="label" style="" transform="translate(-100, -24)"><rect/><foreignObject width="200" height="48"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>xHeight = (baseline_y &#x2212; xheight_y) &#xD7; scale</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-UPM-5" transform="translate(293, 503)"><rect class="basic label-container" style="" x="-130" y="-39" width="260" height="78"/><g class="label" style="" transform="translate(-100, -24)"><rect/><foreignObject width="200" height="48"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>UPM = ascender + abs(descender)</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-AW_MONO-6" transform="translate(1068, 363)"><rect class="basic label-container" style="" x="-130" y="-39" width="260" height="78"/><g class="label" style="" transform="translate(-100, -24)"><rect/><foreignObject width="200" height="48"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>advance_w (mono) = cell_width &#xD7; scale</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-AW_PROP-7" transform="translate(1378, 363)"><rect class="basic label-container" style="" x="-130" y="-39" width="260" height="78"/><g class="label" style="" transform="translate(-100, -24)"><rect/><foreignObject width="200" height="48"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>advance_w (prop) = (RSB_x &#x2212; LSB_x) &#xD7; scale per glyph</p></span></div></foreignObject></g></g></g></g></g></svg>

### Explicit override (per-weight)

When `metrics_override` is present in the weight dict, the build script uses those values directly:

```python
"metrics_override": {
    "asc": 881,      # ascender (font units, positive)
    "desc": -122,    # descender (font units, negative)
    "capH": 739,     # cap height — also determines scale
    "xH": 552,       # x-height (optional, derived if omitted)
}
```

Scale is derived from `capH / (baseline_y - capline_y)`. UPM = `asc + abs(desc)`.

Use overrides when auto-derived values don't match your intended metrics (e.g., hand-tuned ascender that includes extra headroom above the capline).

**Note:** The build script reads reference line positions from the SVG by matching line colors. The designer can adjust these lines in the editor to fine-tune metrics. The config values are only used for initial sheet generation.

---

## Config Reference

### Required Fields

| Field | Type | Description |
|---|---|---|
| `family_name` | str | Font family name (e.g., "My Sans") |
| `font_type` | str | `"monospace"` or `"proportional"` |
| `version` | str | Font version (e.g., "1.0") |
| `copyright` | str | Copyright notice |
| `vendor` | str | 4-character vendor ID |
| `designer` | str | Designer name |
| `description` | str | Font description |
| `url` | str | Designer/vendor URL |
| `weights` | list | List of weight dicts (see below) |
| `grid` | dict | Grid layout parameters (see below) |
| `reference_lines` | dict | Reference line definitions (see below) |
| `codepoints` | list | Flat list of characters to include |

### Weight Dict

| Field | Type | Description |
|---|---|---|
| `name` | str | Human-readable name (e.g., "Bold") |
| `style` | str | OS/2 subfamily name |
| `ps_suffix` | str | PostScript name suffix |
| `weight_class` | int | usWeightClass (100-900) |
| `filename` | str | Output SVG filename |
| `metrics_override` | dict | Optional. Bypass auto-derivation (see below) |

### Metrics Override Dict (optional, per-weight)

When present, the build script uses these values directly instead of deriving from cell geometry. The scale factor is derived from `capH / (baseline_y - capline_y)`.

| Field | Type | Description |
|---|---|---|
| `asc` | int | Ascender in font units (positive) |
| `desc` | int | Descender in font units (negative) |
| `capH` | int | Cap height in font units |
| `xH` | int | x-height in font units (optional, derived if omitted) |

Use this when you have hand-tuned metrics that don't match the auto-derivation (which computes ascender from cell-top to baseline).

### Grid Dict

| Field | Type | Description |
|---|---|---|
| `columns` | int | Glyphs per row |
| `cell_width` | int | Drawing area width (px) |
| `cell_height` | int | Drawing area height (px) |
| `col_gap` | int | Horizontal gap between cells (px) |
| `row_gap` | int | Vertical gap between cells (px) |
| `label_height` | int | Height for codepoint label (px) |
| `margin_left` | int | Left margin (px) |
| `margin_top` | int | Top margin (px) |
| `margin_right` | int | Right margin (px) |
| `margin_bottom` | int | Bottom margin (px) |

### Reference Line Dict

Each key (capline, xheight, baseline, descender) maps to:

| Field | Type | Description |
|---|---|---|
| `y` | int | Initial Y position from cell top (px) |
| `color` | str | Hex color for the line |
| `label` | str | Human-readable label |

### Appearance Dict (optional)

Controls visual styling of the generated SVG sheet. All fields have sensible defaults.

| Field | Type | Default | Description |
|---|---|---|---|
| `background` | str | `"#1A1A1A"` | Background fill color |
| `cell_border_color` | str | `"#333333"` | Cell rectangle stroke color |
| `cell_border_width` | float | `0.5` | Cell rectangle stroke width |
| `label_color` | str | `"#666666"` | Codepoint label text color |
| `label_font_size` | int | `7` | Label font size (px) |
| `ref_line_width` | float | `0.5` | Reference line stroke width |
| `ref_line_opacity` | float | `0.6` | Reference line opacity |
| `marker_line_width` | float | `0.5` | LSB/RSB marker stroke width |
| `marker_opacity` | float | `0.6` | LSB/RSB marker opacity |

### Output Dict (optional)

| Field | Type | Default | Description |
|---|---|---|---|
| `sheets_dir` | str | `"sheets"` | Directory for generated blank SVGs |
| `fonts_dir` | str | `"fonts"` | Directory for built TTF files |

---

## Common Character Sets

### ASCII Printable (94 glyphs)

```python
"codepoints": [chr(c) for c in range(0x21, 0x7F)]
```

### ASCII + Space (95 glyphs)

Space is always added automatically by the build script. You don't need to include it in the sheet — it has no visual glyph.

### Latin-1 Supplement (96 additional glyphs)

```python
# Add to ASCII:
"codepoints": [chr(c) for c in range(0x21, 0x7F)] +
              [chr(c) for c in range(0xA0, 0x100)]
```

### Custom Set

Just list the characters you need:

```python
"codepoints": ["A", "B", "C", "0", "1", "2", "+", "-", "="]
```

---

## Weight Classes

Standard usWeightClass values:

| Value | Name | Description |
|---|---|---|
| 100 | Thin | Hairline |
| 200 | ExtraLight | Ultra Light |
| 300 | Light | |
| 350 | Book | Between Light and Regular |
| 400 | Regular | Normal |
| 500 | Medium | |
| 600 | SemiBold | Demi Bold |
| 700 | Bold | |
| 800 | ExtraBold | Ultra Bold |
| 900 | Black | Heavy |

---

## Troubleshooting

### Glyphs render at wrong position

**Cause:** LSB in `hmtx` table is 0 for all glyphs.
**Fix:** The build script must set LSB = `glyph.xMin` for each glyph, not 0.

### Tall glyphs (braces, brackets) are vertically misaligned

**Cause:** OS/2 table version < 4, or `fsSelection` bit 7 (`USE_TYPO_METRICS`) not set.
**Fix:** Set `OS/2.version = 4` and `fsSelection |= 0x80`.

### Glyph outlines are inverted (filled where they should be empty)

**Cause:** TrueType winding direction is opposite to SVG.
**Fix:** Use `Cu2QuPen(reverse_direction=True)` when converting cubic to quadratic.

### Build script picks up paths from adjacent cells

**Cause:** Stray paths in a `<g>` element that belong to another cell.
**Fix:** Filter paths by checking the first coordinate against the cell's bounding box.

### Proportional font has wrong spacing

**Cause:** LSB/RSB markers are missing or wrong color.
**Fix:** Ensure cyan (`#00FFFF`) vertical line marks left boundary, magenta (`#FF00FF`) marks right boundary in each cell.

---

## File Structure

<svg id="my-svg" width="100%" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" class="flowchart" style="max-width: 2025.37px; background-color: transparent;" viewBox="0 0 2025.3671875 302" role="graphics-document document" aria-roledescription="flowchart-v2"><style>#my-svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;fill:#333;}@keyframes edge-animation-frame{from{stroke-dashoffset:0;}}@keyframes dash{to{stroke-dashoffset:0;}}#my-svg .edge-animation-slow{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 50s linear infinite;stroke-linecap:round;}#my-svg .edge-animation-fast{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 20s linear infinite;stroke-linecap:round;}#my-svg .error-icon{fill:#552222;}#my-svg .error-text{fill:#552222;stroke:#552222;}#my-svg .edge-thickness-normal{stroke-width:1px;}#my-svg .edge-thickness-thick{stroke-width:3.5px;}#my-svg .edge-pattern-solid{stroke-dasharray:0;}#my-svg .edge-thickness-invisible{stroke-width:0;fill:none;}#my-svg .edge-pattern-dashed{stroke-dasharray:3;}#my-svg .edge-pattern-dotted{stroke-dasharray:2;}#my-svg .marker{fill:#333333;stroke:#333333;}#my-svg .marker.cross{stroke:#333333;}#my-svg svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;}#my-svg p{margin:0;}#my-svg .label{font-family:"trebuchet ms",verdana,arial,sans-serif;color:#333;}#my-svg .cluster-label text{fill:#333;}#my-svg .cluster-label span{color:#333;}#my-svg .cluster-label span p{background-color:transparent;}#my-svg .label text,#my-svg span{fill:#333;color:#333;}#my-svg .node rect,#my-svg .node circle,#my-svg .node ellipse,#my-svg .node polygon,#my-svg .node path{fill:#ECECFF;stroke:#9370DB;stroke-width:1px;}#my-svg .rough-node .label text,#my-svg .node .label text,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-anchor:middle;}#my-svg .node .katex path{fill:#000;stroke:#000;stroke-width:1px;}#my-svg .rough-node .label,#my-svg .node .label,#my-svg .image-shape .label,#my-svg .icon-shape .label{text-align:center;}#my-svg .node.clickable{cursor:pointer;}#my-svg .root .anchor path{fill:#333333!important;stroke-width:0;stroke:#333333;}#my-svg .arrowheadPath{fill:#333333;}#my-svg .edgePath .path{stroke:#333333;stroke-width:2.0px;}#my-svg .flowchart-link{stroke:#333333;fill:none;}#my-svg .edgeLabel{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .edgeLabel p{background-color:rgba(232,232,232, 0.8);}#my-svg .edgeLabel rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .labelBkg{background-color:rgba(232, 232, 232, 0.5);}#my-svg .cluster rect{fill:#ffffde;stroke:#aaaa33;stroke-width:1px;}#my-svg .cluster text{fill:#333;}#my-svg .cluster span{color:#333;}#my-svg div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:12px;background:hsl(80, 100%, 96.2745098039%);border:1px solid #aaaa33;border-radius:2px;pointer-events:none;z-index:100;}#my-svg .flowchartTitleText{text-anchor:middle;font-size:18px;fill:#333;}#my-svg rect.text{fill:none;stroke-width:0;}#my-svg .icon-shape,#my-svg .image-shape{background-color:rgba(232,232,232, 0.8);text-align:center;}#my-svg .icon-shape p,#my-svg .image-shape p{background-color:rgba(232,232,232, 0.8);padding:2px;}#my-svg .icon-shape rect,#my-svg .image-shape rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#my-svg .label-icon{display:inline-block;height:1em;overflow:visible;vertical-align:-0.125em;}#my-svg .node .label-icon path{fill:currentColor;stroke:revert;stroke-width:revert;}#my-svg :root{--mermaid-font-family:"trebuchet ms",verdana,arial,sans-serif;}</style><g><marker id="my-svg_flowchart-v2-pointEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 0 L 10 5 L 0 10 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-pointStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="4.5" refY="5" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" orient="auto"><path d="M 0 5 L 10 10 L 10 0 z" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleEnd" class="marker flowchart-v2" viewBox="0 0 10 10" refX="11" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-circleStart" class="marker flowchart-v2" viewBox="0 0 10 10" refX="-1" refY="5" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><circle cx="5" cy="5" r="5" class="arrowMarkerPath" style="stroke-width: 1; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossEnd" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="12" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><marker id="my-svg_flowchart-v2-crossStart" class="marker cross flowchart-v2" viewBox="0 0 11 11" refX="-1" refY="5.2" markerUnits="userSpaceOnUse" markerWidth="11" markerHeight="11" orient="auto"><path d="M 1,1 l 9,9 M 10,1 l -9,9" class="arrowMarkerPath" style="stroke-width: 2; stroke-dasharray: 1, 0;"/></marker><g class="root"><g class="clusters"/><g class="edgePaths"><path d="M575.508,44.343L501.794,51.453C428.081,58.562,280.654,72.781,206.94,85.391C133.227,98,133.227,109,133.227,114.5L133.227,120" id="L_ROOT_CFG_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M575.508,56.534L552.665,61.612C529.823,66.689,484.138,76.845,461.296,85.422C438.453,94,438.453,101,438.453,104.5L438.453,108" id="L_ROOT_SHEETS_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M366.323,190L358.617,194.167C350.911,198.333,335.498,206.667,327.792,214.333C320.086,222,320.086,229,320.086,232.5L320.086,236" id="L_SHEETS_SVG1_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M510.583,190L518.289,194.167C525.996,198.333,541.408,206.667,549.114,214.333C556.82,222,556.82,229,556.82,232.5L556.82,236" id="L_SHEETS_SVG2_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M769.258,56.363L792.412,61.469C815.566,66.576,861.875,76.788,885.029,87.394C908.184,98,908.184,109,908.184,114.5L908.184,120" id="L_ROOT_FONTS_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M859.037,178L847.812,184.167C836.587,190.333,814.137,202.667,802.912,212.333C791.688,222,791.688,229,791.688,232.5L791.688,236" id="L_FONTS_TTF1_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M957.33,178L968.555,184.167C979.78,190.333,1002.23,202.667,1013.455,212.333C1024.68,222,1024.68,229,1024.68,232.5L1024.68,236" id="L_FONTS_TTF2_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M769.258,40.453L907.09,48.211C1044.922,55.968,1320.586,71.484,1458.418,84.742C1596.25,98,1596.25,109,1596.25,114.5L1596.25,120" id="L_ROOT_GGWP_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M1498.477,168.972L1456.742,176.643C1415.008,184.315,1331.539,199.657,1289.805,210.829C1248.07,222,1248.07,229,1248.07,232.5L1248.07,236" id="L_GGWP_SC_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M1548.051,178L1537.042,184.167C1526.034,190.333,1504.017,202.667,1493.008,212.333C1482,222,1482,229,1482,232.5L1482,236" id="L_GGWP_GS_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M1644.449,178L1655.458,184.167C1666.466,190.333,1688.483,202.667,1699.492,212.333C1710.5,222,1710.5,229,1710.5,232.5L1710.5,236" id="L_GGWP_BF_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/><path d="M1694.023,169.767L1733.299,177.306C1772.576,184.845,1851.128,199.922,1890.404,210.961C1929.68,222,1929.68,229,1929.68,232.5L1929.68,236" id="L_GGWP_DOC_0" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" style=";" marker-end="url(#my-svg_flowchart-v2-pointEnd)"/></g><g class="edgeLabels"><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g><g class="edgeLabel"><g class="label" transform="translate(0, 0)"><foreignObject width="0" height="0"><div xmlns="http://www.w3.org/1999/xhtml" class="labelBkg" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="edgeLabel"></span></div></foreignObject></g></g></g><g class="nodes"><g class="node default" id="flowchart-ROOT-0" transform="translate(672.3828125, 35)"><rect class="basic label-container" style="" x="-96.875" y="-27" width="193.75" height="54"/><g class="label" style="" transform="translate(-66.875, -12)"><rect/><foreignObject width="133.75" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>your-font-project/</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-CFG-1" transform="translate(133.2265625, 151)"><rect class="basic label-container" style="" x="-125.2265625" y="-27" width="250.453125" height="54"/><g class="label" style="" transform="translate(-95.2265625, -12)"><rect/><foreignObject width="190.453125" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>my_config.py — your config</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-SHEETS-2" transform="translate(438.453125, 151)"><rect class="basic label-container" style="" x="-130" y="-39" width="260" height="78"/><g class="label" style="" transform="translate(-100, -24)"><rect/><foreignObject width="200" height="48"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table; white-space: break-spaces; line-height: 1.5; max-width: 200px; text-align: center; width: 200px;"><span class="nodeLabel"><p>sheets/&#10;generated blank SVGs</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-SVG1-3" transform="translate(320.0859375, 267)"><rect class="basic label-container" style="" x="-98.390625" y="-27" width="196.78125" height="54"/><g class="label" style="" transform="translate(-68.390625, -12)"><rect/><foreignObject width="136.78125" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>MyFont-Regular.svg</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-SVG2-4" transform="translate(556.8203125, 267)"><rect class="basic label-container" style="" x="-88.34375" y="-27" width="176.6875" height="54"/><g class="label" style="" transform="translate(-58.34375, -12)"><rect/><foreignObject width="116.6875" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>MyFont-Bold.svg</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-FONTS-5" transform="translate(908.18359375, 151)"><rect class="basic label-container" style="" x="-95.078125" y="-27" width="190.15625" height="54"/><g class="label" style="" transform="translate(-65.078125, -12)"><rect/><foreignObject width="130.15625" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>fonts/ — built TTFs</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-TTF1-6" transform="translate(791.6875, 267)"><rect class="basic label-container" style="" x="-96.5234375" y="-27" width="193.046875" height="54"/><g class="label" style="" transform="translate(-66.5234375, -12)"><rect/><foreignObject width="133.046875" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>MyFont-Regular.ttf</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-TTF2-7" transform="translate(1024.6796875, 267)"><rect class="basic label-container" style="" x="-86.46875" y="-27" width="172.9375" height="54"/><g class="label" style="" transform="translate(-56.46875, -12)"><rect/><foreignObject width="112.9375" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>MyFont-Bold.ttf</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-GGWP-8" transform="translate(1596.25, 151)"><rect class="basic label-container" style="" x="-97.7734375" y="-27" width="195.546875" height="54"/><g class="label" style="" transform="translate(-67.7734375, -12)"><rect/><foreignObject width="135.546875" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>ggwp/ — the toolkit</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-SC-9" transform="translate(1248.0703125, 267)"><rect class="basic label-container" style="" x="-86.921875" y="-27" width="173.84375" height="54"/><g class="label" style="" transform="translate(-56.921875, -12)"><rect/><foreignObject width="113.84375" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>sheet_config.py</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-GS-10" transform="translate(1482, 267)"><rect class="basic label-container" style="" x="-97.0078125" y="-27" width="194.015625" height="54"/><g class="label" style="" transform="translate(-67.0078125, -12)"><rect/><foreignObject width="134.015625" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>generate_sheet.py</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-BF-11" transform="translate(1710.5, 267)"><rect class="basic label-container" style="" x="-81.4921875" y="-27" width="162.984375" height="54"/><g class="label" style="" transform="translate(-51.4921875, -12)"><rect/><foreignObject width="102.984375" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>build_fonts.py</p></span></div></foreignObject></g></g><g class="node default" id="flowchart-DOC-12" transform="translate(1929.6796875, 267)"><rect class="basic label-container" style="" x="-87.6875" y="-27" width="175.375" height="54"/><g class="label" style="" transform="translate(-57.6875, -12)"><rect/><foreignObject width="115.375" height="24"><div xmlns="http://www.w3.org/1999/xhtml" style="display: table-cell; white-space: nowrap; line-height: 1.5; max-width: 200px; text-align: center;"><span class="nodeLabel"><p>GLYPHSHEET.md</p></span></div></foreignObject></g></g></g></g></g></svg>

---

## For Agents: Config Validation Checklist

Before generating sheets, verify the config:

- [ ] `family_name` is set and non-empty
- [ ] `font_type` is `"monospace"` or `"proportional"`
- [ ] `vendor` is exactly 4 characters
- [ ] `weights` has at least one entry
- [ ] Each weight has `name`, `style`, `ps_suffix`, `weight_class`, `filename`
- [ ] `weight_class` is between 100 and 900
- [ ] `grid.columns` > 0
- [ ] `grid.cell_width` > 0 and `grid.cell_height` > 0
- [ ] `reference_lines` has capline, xheight, baseline, descender
- [ ] Each reference line has `y`, `color`, `label`
- [ ] `capline.y < xheight.y < baseline.y < descender.y` (top to bottom)
- [ ] `codepoints` is non-empty, no duplicates
- [ ] If `font_type == "proportional"`, `sidebearing_markers` has `lsb_color` and `rsb_color`

---

## For Agents: Build Verification Checklist

After building fonts, verify:

- [ ] TTF file exists and is > 10KB
- [ ] `name` table has correct family name, style, PostScript name
- [ ] `OS/2.version` == 4
- [ ] `OS/2.fsSelection` has bit 7 set (USE_TYPO_METRICS)
- [ ] `OS/2.usWeightClass` matches config
- [ ] `post.isFixedPitch` == 1 (monospace) or 0 (proportional)
- [ ] `hmtx` has correct advance widths (all same for mono, varying for proportional)
- [ ] `hmtx` LSB matches `glyf` xMin for each glyph
- [ ] `cmap` maps all codepoints correctly
- [ ] Glyph count matches codepoint count + 2 (.notdef, space)

---

## Dependencies

- Python 3.8+
- `fontTools` (`pip install fonttools`)
- Any SVG editor for drawing (Affinity Designer, Inkscape, etc.)

---

## Glossary

| Term | Definition |
|---|---|
| **Advance width** | Horizontal distance the cursor moves after rendering a glyph |
| **Ascender** | Distance from baseline to top of tallest glyph (in font units) |
| **Baseline** | The line letters sit on |
| **Cap height** | Height of capital letters (H, E, B) from baseline |
| **Cell** | One rectangle in the glyph sheet grid, containing one glyph |
| **Codepoint** | Unicode number for a character (e.g., U+0041 = "A") |
| **Descender** | Distance from baseline to bottom of lowest glyph (negative, in font units) |
| **Font units** | Internal coordinate system of the font (typically 1000 UPM) |
| **Glyph sheet** | SVG file containing all glyphs laid out on a grid |
| **LSB** | Left sidebearing — space between glyph origin and leftmost pixel |
| **RSB** | Right sidebearing — space between rightmost pixel and advance width |
| **Scale** | Conversion factor from SVG pixels to font units |
| **UPM** | Units per em — the font's coordinate space size (ascender - descender) |
| **x-height** | Height of lowercase letters (x, o, a) from baseline |

---

**GGWP.**
