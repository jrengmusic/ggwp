# GGWP: GlyphSheet Generation Workflow Protocol

**Version 1.0**

You see this guide because you ran `ggwp --help`.

GGWP reads your metrics and names (a manifest), and your drawings (a glyph sheet), and writes an OpenType font with TrueType outlines. It knows nothing about type design. It reads tables, transforms artwork into font units, and writes tables — nothing else.

---

## Command Line

Arguments select what runs. They never carry font rules.

- `ggwp --build=<manifest.md> --weight=<table> --out=<font.ttf>` — read the manifest, read the weight's glyph sheet, write the font, then validate it
- `ggwp --init=<font-name>` — create whatever is missing, then reconcile the glyph sheets against the manifest's glyph table
- `ggwp --extract=<font.ttf> --manifest=<manifest.md> --weight=<table>` — read an existing font, then write that weight's glyph sheet
- `ggwp --version` — the version number
- `ggwp --help` — this guide

Each long option takes its value after an equals sign. A space does not work.

`--weight` names a table in the manifest, not a number. `--weight` matches a table when the value and the table name give the same tag form. The tag form splits a name at each space and at each change from a lower-case letter to an upper-case letter, makes each word upper case, and joins the words with underscores. `regular`, `Regular` and `REGULAR` all give `REGULAR`. `semiBold` and `SemiBold` give `SEMI_BOLD`, and `semibold` gives `SEMIBOLD`, so it does not match them. `--out` is relative to the current directory. The glyph sheet is relative to the manifest's own directory.

---

## The `--init` Contract

The `## glyphs` table is the single source of truth for which glyphs exist. `--init` creates whatever is missing, then reconciles whatever exists against that table.

- When neither the manifest nor the regular weight's glyph sheet exists, `--init` writes both.
- When the manifest exists without a `## glyphs` table, and a glyph sheet exists, `--init` builds the table from that sheet.
- When the manifest exists without a `## glyphs` table, and no sheet exists, `--init` writes the default table. The sheet stays empty.
- When the manifest exists without a `## guides` table, `--init` adds the table with the four default rows.
- When the manifest exists without a `## colour` table, `--init` adds the table with the default rows.
- When both the manifest and a glyph sheet exist, the `## glyphs` table drives the sheet: a row with no matching cell gains one, a cell with no matching row is removed along with its artwork, and the remaining cells repack and the canvas resizes.
- `--init` rewrites the sheet's markup on every run. Artwork in a cell that does not move keeps its geometry relative to that cell. GGWP's writer always sets the surrounding markup — indentation, attribute order, and self-closing form. `--init` shifts `path`, `rect`, `ellipse`, `circle`, and `line` tags to follow a moved cell. `--init` fails when a moved cell holds any other tag.
- `--init` runs the reconciliation over every weight the manifest declares, in one pass.
- Running `--init` twice over the same inputs produces the same files.

`--init` derives the manifest's file name from the family name. It lowercases the name and replaces every space with a hyphen, then appends `.md`. The file `--init` writes lands in the current working directory. `Display Serif` becomes `display-serif.md`.

---

## The `--extract` Contract

`--extract` reads an existing font and writes one weight's glyph sheet.

- `--extract`, `--manifest`, and `--weight` are all required. The font file and the manifest file must exist. `--weight` names a table of the manifest, and that table's `svg` path decides where the glyph sheet lands.
- `--extract` scales each outline and each advance by 1024 divided by the source font's units per em.
- `--extract` walks the font's character map. It skips every character at or below the space: the notdef, the control characters, and the space. It writes each other character as a single-codepoint cell.
- A single glyph's cell width is its scaled advance, divided by the plain advance, times `cellWidth`. A build of that sheet gives the same advance back.
- `--extract` finds ligatures by shaping. It collects every character that is the input of a `GSUB` lookup. It shapes every sequence of two of these characters, and every sequence of three. A sequence that shapes to exactly one glyph is a ligature. `--extract` writes it as a multi-codepoint cell.
- A ligature's cell width is the width of its codepoint count in plain cells, plus one gutter between the cells. A build ignores a ligature's own cell width.
- `--extract` fails on a glyph above the space that has an advance and no outline. The failure is `glyph has no outline`, and it names the codepoint in hexadecimal.
- `--extract` fails when a `GSUB` lookup outputs a glyph that has no character and that no ligature uses. The failure is `substitution output unmapped`, and it names the glyph index.
- The manifest must exist. `--extract` reads its metrics and never writes them.
- When the manifest has no `## glyphs` table, `--extract` writes one from the extracted cells. A manifest that has the table is never rewritten.
- `--extract` lays out the sheet from the manifest's `## glyphs` table, as `--init` does. Only a cell that the table lists is written.

---

## The Two Inputs

| Artifact | What it is |
| --- | --- |
| `<name>.md` | the manifest — metrics, names, and one table per weight |
| `<sheet>.svg` | the glyph sheet — one drawing per glyph |

One manifest declares every weight. One glyph sheet holds one weight.

---

## The Manifest

The manifest is a markdown file of grid tables. Table names are headings. The `metrics`, `font`, and weight tables each have a `key`, a `type`, and a `value` column. The `glyphs`, `guides` and `colour` tables have their own column sets.

### metrics

One table named `## metrics`, shared by every weight. Every metric is in font units at 1024 units per em.

| Key | What it sets |
| --- | --- |
| `ascender` | the ascender, in font units |
| `descender` | the descender, negative, in font units |
| `lineGap` | the line gap, in font units |
| `capHeight` | the cap height, in font units |
| `xHeight` | the x-height, in font units |
| `baseline` | the baseline's position in the glyph sheet's own units |
| `capline` | the cap line's position in the glyph sheet's own units |
| `cellWidth` | the width of one sheet cell, in the sheet's own units |
| `advanceWidthFactor` | the advance width, as a multiple of `cellWidth` |

The units per em is 1024 for every font GGWP writes. It is a contract constant, not a manifest value.

One value is derived and never authored. The sheet scale is `capHeight` divided by `baseline` minus `capline`.

`capHeight` and `baseline` also appear in the `## guides` table, as `key` values. The two copies serve different purposes. The `## metrics` pair sets the font's scale. The `## guides` pair places the drawn reference line on the sheet.

### font

One table named `## font`, shared by every weight.

| Key | What it sets |
| --- | --- |
| `copyright` | the copyright notice |
| `family` | the family name |
| `trademark` | the trademark notice |
| `manufacturer` | the manufacturer name |
| `designer` | the designer name |
| `description` | the description |
| `url` | the vendor and designer address |
| `version` | the version number |

### glyphs

One table named `## glyphs`, shared by every weight. It is the single source of truth for which glyphs exist — see The `--init` Contract.

| Column | What it holds |
| --- | --- |
| `codepoints` | the glyph itself, enclosed in backticks |

One row is one glyph. A cell holding more than one character is a ligature. The default table spans the printable ASCII range, U+0021 through U+007E — 94 rows.

### guides

One table named `## guides`, shared by every weight.

| Column | What it holds |
| --- | --- |
| `key` | the metric the reference line marks |
| `offset` | the line's position, measured down from the top of a cell |

One row is one reference line. The default table has four rows: `capHeight`, `xHeight`, `baseline`, and `descender`. The line's colour comes from the `## colour` row with the same key.

### colour

One table named `## colour`, shared by every weight. It holds every colour of the glyph sheet.

| Column | What it holds |
| --- | --- |
| `key` | the part of the sheet the colour paints |
| `colour` | the colour, as a hex value |

One row is one colour. The default table has nine rows.

| Key | What it paints |
| --- | --- |
| `background` | the sheet's background rectangle |
| `cell` | the outline of each cell |
| `label` | the codepoint label of each cell |
| `glyph` | the fill of each glyph's artwork |
| `advance` | the cell guides: the unit-cell edges and the advance |
| `capHeight` | the cap height reference line |
| `xHeight` | the x-height reference line |
| `baseline` | the baseline reference line |
| `descender` | the descender reference line |

A `## guides` row reads its line colour from the `## colour` row with the same key. A missing row is fatal.

### weight tables

One table per weight. The table's own name is what `--weight` selects.

| Key | What it sets |
| --- | --- |
| `svg` | the glyph sheet's path, relative to the manifest |
| `style` | the style name |
| `fullName` | the full name |
| `postScriptName` | the PostScript name |
| `weightClass` | the weight class, 100 to 900 |
| `ascender` | the ascender for this weight only, in font units |
| `descender` | the descender for this weight only, negative, in font units |
| `lineGap` | the line gap for this weight only, in font units |

A weight table may carry its own `ascender`, `descender` and `lineGap` rows. They replace the `## metrics` values for that weight only.

A style of `Bold` sets the bold bit in `head` and in `OS/2`. Every other style is regular.

GGWP writes two platforms into the `name` table, and they diverge for a style outside `Regular`, `Bold`, `Italic`, and `Bold Italic`. The Macintosh records always carry the true family name and the true style name. The Windows records fold a non-RIBBI style into the family name — `"{family} {style}"` — and set the Windows style name to `Regular`.

---

## The Glyph Sheet

The glyph sheet is an SVG file. A generated sheet has three layers: a background rectangle, a `<g id="guides">` layer, and the cell groups. The guides layer holds each cell's label, its reference lines, and its cell guides. GGWP ignores it when it reads a sheet back.

The cell guides are vertical lines. One line marks each unit-cell edge inside the glyph. One line marks the glyph's advance. GGWP draws each line where the build places it. A single glyph has one line, at its advance. A ligature of two or three characters also has a line at each unit-cell edge between its characters.

Each glyph group carries the glyph fill. Every artwork element in the group inherits it.

Your own top-level elements survive `--init`. GGWP carries through every top-level element that is neither a glyph group nor the guides layer. `<defs>` and `<title>` are the usual cases. A top-level `<rect>` is the background, and GGWP resizes it to the new canvas instead of replacing it. GGWP writes a background rectangle only when the sheet has none.

A `<g>` element is a glyph cell only when its id starts with an underscore, followed by one or more four-digit hexadecimal codepoints. An id that only looks like hex, without the leading underscore, passes through like any other element.

Each glyph is a `<g>` element with an `id`. The group holds one `<rect>`, which is the glyph's cell, and the artwork that follows it.

An id is an underscore and then one hexadecimal codepoint of four digits:

```
_0041    the letter A
_0061    the letter a
_2122    the trade mark sign
```

More than one codepoint in one id makes a ligature:

```
_0066_0069    the f-i ligature
```

Two ids are reserved. `_0000` is the notdef glyph and `_0020` is the space. GGWP writes both into the font. `--init` rejects a `## glyphs` row that holds either character.

The cell rectangle does two things. Its position places the artwork. Its width sets a single glyph's advance width. Cell rectangles carry fractional coordinates, written to three decimal places.

A single glyph's advance is its cell width divided by `cellWidth`, times the plain advance. The plain advance is `cellWidth` × `advanceWidthFactor`. GGWP rounds the result to the nearest unit. A cell wider than `cellWidth` gives a proportionally wider glyph.

A ligature ignores its own cell width. Its advance is the plain advance width, multiplied by its codepoint count.

A cell with no artwork produces no glyph.

When ligatures share a first character, GGWP matches the longest ligature first.

GGWP orders the glyphs by id — the shorter ids first, then alphabetically. Single glyphs therefore come before ligatures, and the character map stays in one run.

---

## What GGWP Writes

GGWP writes an sfnt file with TrueType outlines. It writes these tables:

| Table | What it carries |
| --- | --- |
| `head` | units per em, bounding box, bold bit, version |
| `hhea` | ascender, descender, line gap, bearing extremes |
| `maxp` | glyph count, point and contour maxima |
| `hmtx` | one advance width and one left side bearing per glyph |
| `post` | version 2.0, glyph names, fixed pitch |
| `cmap` | format 4, plus format 12 for codepoints above U+FFFF |
| `name` | Macintosh and Windows records, one pair per name |
| `OS/2` | version 4, weight class, PANOSE, vertical metrics |
| `glyf` | the outlines |
| `loca` | short offsets into `glyf` |
| `GSUB` | the ligature substitutions |

`GSUB` appears only when the sheet holds a ligature.

GGWP derives the `post` fixed pitch flag and the PANOSE proportion from the glyphs. A font is fixed pitch when every single glyph's advance is a whole multiple of the plain advance. A glyph that fills two cells keeps the font fixed pitch.

Cubic curves become quadratic curves, to a maximum error of one font unit. Each contour is reversed, so the outer contours wind clockwise. A contour never repeats its first point.

---

## Validation

GGWP loads the font it just wrote back through FreeType, and checks it against the manifest:

- the glyph count
- every non-ligature glyph's character map entry
- every glyph's advance width
- every glyph with drawn contours, that FreeType still reports an outline

A failed check is fatal, and it names the glyph and the two values.

---

## Determinism and Failure

Your manifest, your glyph sheet, and the binary determine the output bytes. No timestamps, no paths, no host state.

A second run over the same inputs produces the same file.

Every failure is fatal and exits non-zero.
