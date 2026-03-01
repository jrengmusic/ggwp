# GGWP — Glyph Generator Workflow Protocol

**Version:** 1.0
**Purpose:** A formalized pipeline for creating fonts from SVG glyph sheets.

---

## What Is This?

GGWP is a method for building TrueType fonts (.ttf) without a GUI font editor. The designer draws glyphs in any SVG editor (Affinity Designer, Inkscape, etc.) on a structured grid sheet. A build script reads the sheet and produces a valid TTF with correct metrics, naming, and encoding.

**The glyph sheet IS the single source of truth.** Everything the build script needs — outlines, metrics, spacing — is encoded visually in the SVG.

---

## The Chain

```
1. CONFIG         Define font metadata, grid, codepoints, weights
                  (Python dict in a .py file)
                       |
                       v
2. GENERATE       Run generate_sheet.py to emit blank SVG sheets
                  (one sheet per weight)
                       |
                       v
3. DRAW           Designer fills glyphs into cells using any SVG editor
                  (Affinity Designer, Inkscape, etc.)
                       |
                       v
4. BUILD          Run build_fonts.py to parse filled sheets into TTFs
                  (reads everything from the SVG — no manual metric entry)
```

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
- `scale = capH / (baseline_y - capline_y)` where `capH` is the target cap height in font units
- `ascender = (baseline_y - cell_top) * scale` (derived from reference line positions)
- `descender = (baseline_y - descender_y) * scale` (negative value)
- `UPM = ascender - descender`

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

    "codepoints": [
        "A", "B", "C", ...  # your character set
    ],

    "output": {
        "sheets_dir": "sheets",
        "fonts_dir": "fonts",
    },
}
```

### Step 5: Generate Blank Sheets

```bash
python3 ggwp/generate_sheet.py my_config.py
```

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
python3 ggwp/build_fonts.py my_config.py
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

```
+------------------------------------------------------------------+
|  Title: "My Sans Regular"                     Legend: -- -- -- -- |
|                                                                   |
|  +--- label ---+    +--- label ---+                               |
|  |  A          |    |  B          |    ...                        |
|  |  ========== | <- capline (purple)                              |
|  |             |                                                  |
|  |  ---------- | <- x-height (green)                              |
|  |             |                                                  |
|  |  ========== | <- baseline (yellow)                             |
|  |             |                                                  |
|  |  ---------- | <- descender (red)                               |
|  +-------------+    +-------------+                               |
|                                                                   |
|  +--- label ---+    +--- label ---+                               |
|  |  K          |    |  L          |    ...                        |
|  |  ...        |    |  ...        |                               |
+------------------------------------------------------------------+
```

For proportional fonts, each cell also has:
```
|  |  |                          |  |  |
|  |  |        glyph             |  |  |
|  |  |                          |  |  |
   ^                                ^
   LSB (cyan)                       RSB (magenta)
   
   advance_width = RSB_x - LSB_x (in SVG px, scaled to font units)
```

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

All metrics are derived from the SVG reference lines. Nothing is hardcoded.

```
Given (from SVG, row 0):
    capline_y   = Y position of purple line
    xheight_y   = Y position of green line
    baseline_y  = Y position of yellow line
    descender_y = Y position of red line

Derived:
    capH  = target cap height in font units (e.g., 739)
    scale = capH / (baseline_y - capline_y)

    ascender  = round((baseline_y - capline_y) * scale)    = capH
    descender = -round((descender_y - baseline_y) * scale) = negative
    xHeight   = round((baseline_y - xheight_y) * scale)
    UPM       = ascender - descender

    advance_w (mono) = round(cell_width * scale)
    advance_w (prop) = round((RSB_x - LSB_x) * scale)  per glyph
```

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

```
your-font-project/
|-- my_config.py              <-- your config (copy from sheet_config.py)
|-- sheets/                   <-- generated blank SVGs
|   |-- MyFont-Regular.svg
|   |-- MyFont-Bold.svg
|-- fonts/                    <-- built TTFs
|   |-- MyFont-Regular.ttf
|   |-- MyFont-Bold.ttf
|-- ggwp/                     <-- the toolkit (don't modify)
    |-- sheet_config.py       <-- default/example config
    |-- generate_sheet.py     <-- blank sheet generator
    |-- build_fonts.py        <-- SVG-to-TTF builder
    |-- GLYPHSHEET.md         <-- this document
```

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
