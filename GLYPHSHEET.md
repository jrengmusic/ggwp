# GGWP — GlyphSheet Generation Workflow Protocol

**Version:** 2.0
**Purpose:** A formalized pipeline for creating fonts from SVG glyph sheets.

---

## What Is This?

GGWP is a method for building TrueType fonts (.ttf) without a GUI font editor. The designer draws glyphs in any SVG editor (Affinity Designer, Inkscape, etc.) on a structured grid sheet. A build script reads the sheet and produces a valid TTF with correct metrics, naming, and encoding.

**The glyph sheet IS the single source of truth.** Everything the build script needs — outlines, cell geometry — is encoded in the SVG. Metrics come from `sheet_config.py`.

---

## The Chain

```
1. CONFIG         Define font metadata, grid, codepoints, weights
                  (flat module-level constants in sheet_config.py)
                       |
                       v
2. GENERATE       Run generate_unified_sheets.py to emit blank SVG sheets
                  (one sheet per weight, written to sheets/)
                       |
                       v
3. DRAW           Designer fills glyphs into cells using any SVG editor
                  (Affinity Designer, Inkscape, etc.)
                       |
                       v
4. BUILD          Run build_fonts.py to parse filled sheets into TTFs
                  (reads cell geometry from SVG, metrics from config)
```

---

## SVG Structure (v2 Format)

Every generated sheet has exactly three layers:

```xml
<svg xmlns="http://www.w3.org/2000/svg" width="W" height="H">

  <!-- Layer 1: Background -->
  <rect width="W" height="H" fill="rgb(20,20,20)"/>

  <!-- Layer 2: Guides (lockable, hideable, deletable) -->
  <g id="guides">
    <text x="42" y="78" style="font-size:7px;fill:rgb(100,100,100)">A</text>
    <path d="M 40,101 L 160,101" style="fill:none;stroke:rgb(167,139,250);stroke-width:0.5"/>
    <path d="M 40,141 L 160,141" style="fill:none;stroke:rgb(52,211,153);stroke-width:0.5"/>
    <path d="M 40,246 L 160,246" style="fill:none;stroke:rgb(250,204,21);stroke-width:0.5"/>
    <path d="M 40,287 L 160,287" style="fill:none;stroke:rgb(248,113,113);stroke-width:0.5"/>
    <path d="M 100,70 L 100,313" style="fill:none;stroke:rgb(68,68,68);stroke-width:0.5px;stroke-dasharray:2,3"/>
    <!-- ... more labels, reflines, centerlines for every cell ... -->
  </g>

  <!-- Layer 3: Cell groups (structural truth) -->
  <g id="_0041">
    <rect x="40" y="70" width="120" height="243"
      style="fill:none;stroke:rgb(50,50,50);stroke-width:0.5"/>
    <!-- designer draws glyph artwork here -->
  </g>

  <g id="_0042">
    <rect x="170" y="70" width="120" height="243"
      style="fill:none;stroke:rgb(50,50,50);stroke-width:0.5"/>
  </g>

  <!-- 2-cell ligature -->
  <g id="_002D_003E">
    <rect x="40" y="2600" width="250" height="243"
      style="fill:none;stroke:rgb(50,50,50);stroke-width:0.5"/>
  </g>

  <!-- 3-cell ligature -->
  <g id="_003C_003D_003E">
    <rect x="40" y="3860" width="380" height="243"
      style="fill:none;stroke:rgb(50,50,50);stroke-width:0.5"/>
  </g>

</svg>
```

---

## Cell Identity (`_XXXX` id Format)

Each cell `<g>` uses a leading `_` prefix followed by zero-padded 4-digit uppercase hex Unicode codepoints, separated by `_`.

The `_` prefix ensures XML-safe ids (XML ids cannot start with a digit) and survives vector editor round-trips (Affinity Designer, Inkscape).

| Character | id |
|---|---|
| `!` | `_0021` |
| `A` | `_0041` |
| `->` | `_002D_003E` |
| `<=>` | `_003C_003D_003E` |

**Generation:**
```python
def char_id(ch):
    return "_" + "_".join(f"{ord(c):04X}" for c in ch)
```

**Parsing algorithm:**
```python
parts = [p for p in g.get("id").split("_") if p]
char = "".join(chr(int(p, 16)) for p in parts)
```

Single part = single character. Multiple parts = ligature.

---

## Cell Geometry

Each cell `<g>` contains exactly one `<rect>` as its first child. This rect is the **true coordinate source** — the parser reads cell position and size directly from its attributes.

| Attribute | Meaning |
|---|---|
| `x` | Cell left edge (px) |
| `y` | Cell top edge (px) |
| `width` | Cell width — `CELL_W` for single chars, `N*CELL_W + (N-1)*GUTTER` for ligatures |
| `height` | Cell height — always `CELL_H` |

No grid math from config is needed at parse time. The rect is the ground truth.

---

## Guides Layer

The `<g id="guides">` layer contains all visual aids:

- **Labels** — `<text>` elements showing the character
- **Reflines** — horizontal `<path>` elements (capline, x-height, baseline, descender)
- **Centerlines** — vertical dashed `<path>` elements

The guides layer is:
- **Lockable** in vector editors (prevents accidental edits)
- **Hideable** (declutters the workspace)
- **Deletable** — fonts build correctly without it

The build script ignores the guides layer entirely (`id == "guides"` → skip).

---

## Glyph Artwork

Everything inside a cell `<g>` that is NOT the cell `<rect>` is glyph artwork.

Supported artwork elements:
- `<path>` with `d` attribute — filled outlines (most glyphs)
- `<rect>` — rectangular glyphs (-, _, |, .)

The parser collects all non-cell-rect children as glyph data.

---

## Parser Algorithm

```
1. Parse SVG with xml.etree.ElementTree
2. For each direct child <g> of <svg>:
   a. Read id attribute
   b. Skip if id == "guides"
   c. Split id by "_", filter empty parts
   d. Validate each part as a hex codepoint — skip with WARNING if invalid
   e. Decode: char = "".join(chr(int(p, 16)) for p in parts)
   f. Find first <rect> child — this is the cell rect
   g. Read x, y, width, height from rect attributes
   h. Collect all other children as glyph artwork:
      - <path> with d attribute -> ("path", d_string)
      - <rect> (non-cell) -> ("rect", (x, y, w, h))
   i. If cell has no artwork: print INFO, skip (do not add to glyphs dict)
   j. Store: char -> { rect: (x, y, w, h), artwork: [...] }
3. Compute scale from config: CAP_H / (BASELINE_Y - CAPLINE_Y)
4. For each glyph, compute transform from cell rect position + scale
5. Return glyphs dict and scale
```

---

## Validation

The build script warns on:

- `<g>` with invalid id (not hex codepoints) — WARNING, skipped
- `<g>` with no `<rect>` child — WARNING, skipped
- `<g>` with `<rect>` but no artwork — INFO, skipped (empty cell produces no glyph)
- Ungrouped `<path>` with glyph fill color outside any cell `<g>` — WARNING (artwork drawn outside group)

---

## Generator Output

`generate_unified_sheets.py` produces per weight:

1. Background `<rect>`
2. `<g id="guides">` containing all labels, reflines, and centerlines for every cell
3. One `<g id="_XXXX">` per character, each containing one `<rect>`
4. One `<g id="_XXXX_YYYY">` per 2-cell ligature (width = `CELL_W * 2 + GUTTER`)
5. One `<g id="_XXXX_YYYY_ZZZZ">` per 3-cell ligature (width = `CELL_W * 3 + GUTTER * 2`)

Ligature rows are appended after all single-character rows.

---

## Metrics

All metrics come from `sheet_config.py` constants — not derived from SVG geometry.

| Constant | Meaning |
|---|---|
| `CAP_H` | Cap height in font units |
| `X_H` | x-height in font units |
| `ASC` | Ascender in font units (positive) |
| `DESC` | Descender in font units (negative) |
| `LINE_GAP` | sTypoLineGap |
| `CAPLINE_Y` | Absolute Y of capline in SVG (row 0) |
| `BASELINE_Y` | Absolute Y of baseline in SVG (row 0) |
| `ADVANCE_W_FACTOR` | `advance_w = round(CELL_W * ADVANCE_W_FACTOR)` |

Scale: `CAP_H / (BASELINE_Y - CAPLINE_Y)`

Transform per cell: `baseline_svg = cell_y + (BASELINE_Y - MARGIN_TOP)`

---

## Config Reference

`sheet_config.py` uses flat module-level constants (not a nested `CONFIG` dict).

### Grid

| Constant | Description |
|---|---|
| `COLS` | Glyphs per row |
| `MARGIN_TOP` | Top margin (px) |
| `MARGIN_LEFT` | Left margin (px) |
| `CELL_W` | Cell width (px) |
| `CELL_H` | Cell height (px) |
| `GUTTER` | Gap between cells (px) |
| `COL_STEP` | `CELL_W + GUTTER` (derived) |
| `ROW_STEP` | `CELL_H + GUTTER` (derived) |

### Reference Lines

| Constant | Description |
|---|---|
| `REFLINES` | List of `(offset_from_cell_top, color)` tuples |
| `CAPLINE_Y` | `MARGIN_TOP + capline_offset` |
| `BASELINE_Y` | `MARGIN_TOP + baseline_offset` |

### Appearance

| Constant | Description |
|---|---|
| `BG_COLOR` | SVG background fill |
| `CELL_STROKE` | Cell rect stroke color |
| `LABEL_COLOR` | Guide label text color |
| `LABEL_FONT_SIZE` | Guide label font size (px) |
| `GLYPH_FILL` | Expected fill color for glyph artwork |
| `REFLINE_COLORS` | Set of refline colors (derived from REFLINES) |

### Font Metadata

| Constant | Description |
|---|---|
| `FAMILY` | Font family name |
| `VERSION` | Font version string |
| `COPYRIGHT` | Copyright notice |
| `TRADEMARK` | Trademark string |
| `VENDOR` | 4-char vendor ID (achVendID) |
| `DESIGNER` | Designer name |
| `DESCRIPTION` | Font description |
| `URL` | Designer/vendor URL |

### Codepoints and Ligatures

| Constant | Description |
|---|---|
| `CHARS` | Flat list of single characters |
| `LIGATURES_2` | List of 2-character ligature strings (empty = disabled) |
| `LIGATURES_3` | List of 3-character ligature strings (empty = disabled) |

### Weights

`WEIGHTS` is a list of dicts, each with:

| Key | Description |
|---|---|
| `svg` | SVG filename (in `sheets/`) |
| `style` | Style name (e.g., "Regular", "Bold") |
| `full` | Full font name |
| `ps` | PostScript name |
| `weight_class` | usWeightClass (100–900) |

---

## fsSelection

| Style | fsSelection | Meaning |
|---|---|---|
| Regular (and non-RIBBI) | `0xC0` | USE_TYPO_METRICS + REGULAR |
| Bold | `0xA0` | USE_TYPO_METRICS + BOLD |

---

## Win Name Table (non-RIBBI weights)

For weights outside Regular/Bold/Italic/Bold Italic, the build script patches the Windows platform name records:

- Name ID 1 (Family): `"{FAMILY} {style}"` (e.g., "My Font Book")
- Name ID 2 (Subfamily): `"Regular"`

This ensures correct grouping in Windows font menus.

---

## File Structure

```
your-font-project/
|-- sheet_config.py               <-- config (copy and modify)
|-- generate_unified_sheets.py    <-- blank sheet generator
|-- build_fonts.py                <-- SVG-to-TTF builder
|-- GLYPHSHEET.md                 <-- this document
|-- sheets/                       <-- generated blank SVGs
|   |-- GlyphSheet-Regular.svg
|-- fonts/                        <-- built TTFs
|   |-- MyFont-Regular.ttf
```

---

## For Agents: Config Validation Checklist

Before generating sheets, verify:

- [ ] `FAMILY` is set and non-empty
- [ ] `VENDOR` is exactly 4 characters
- [ ] `WEIGHTS` has at least one entry
- [ ] Each weight has `svg`, `style`, `full`, `ps`, `weight_class`
- [ ] `weight_class` is between 100 and 900
- [ ] `COLS` > 0
- [ ] `CELL_W` > 0 and `CELL_H` > 0
- [ ] `REFLINES` has at least capline and baseline entries
- [ ] `CAPLINE_Y < BASELINE_Y`
- [ ] `CHARS` is non-empty, no duplicates
- [ ] `LIGATURES_2` entries are exactly 2 characters each
- [ ] `LIGATURES_3` entries are exactly 3 characters each

---

## For Agents: Build Verification Checklist

After building fonts, verify:

- [ ] TTF file exists and is > 10KB
- [ ] `name` table has correct family name, style, PostScript name
- [ ] `OS/2.version` == 4
- [ ] `OS/2.fsSelection`: `0xC0` for Regular/non-RIBBI, `0xA0` for Bold
- [ ] `OS/2.usWeightClass` matches config
- [ ] `post.isFixedPitch` == 1 (monospace)
- [ ] `hmtx` advance widths are all equal (monospace)
- [ ] `hmtx` LSB matches `glyf` xMin for each glyph
- [ ] `cmap` maps all codepoints correctly
- [ ] Glyph count matches `CHARS` count + 2 (.notdef, space)

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
| **Ascender** | Distance from baseline to top of tallest glyph (font units) |
| **Baseline** | The line letters sit on |
| **Cap height** | Height of capital letters from baseline |
| **Cell** | One rectangle in the glyph sheet grid, containing one glyph |
| **Codepoint** | Unicode number for a character (e.g., U+0041 = "A") |
| **Descender** | Distance from baseline to bottom of lowest glyph (negative, font units) |
| **Font units** | Internal coordinate system of the font (UPM-based) |
| **Glyph sheet** | SVG file containing all glyphs laid out on a grid |
| **Scale** | Conversion factor from SVG pixels to font units |
| **UPM** | Units per em — `ASC - DESC` |
| **x-height** | Height of lowercase letters from baseline |

---

**GGWP.**
