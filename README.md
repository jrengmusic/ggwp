# GGWP — GlyphSheet Generation Workflow Protocol

**Build fonts from SVG glyph sheets. No GUI font editor required.**

GUI font editors are drawing tools. But if you already have a better drawing environment — Affinity Designer, Figma, Inkscape — the only thing missing is the SVG-to-TTF conversion. GGWP fills that gap.

`ggwp` is a single native command-line tool. It reads your metrics and names (a manifest), and your drawings (a glyph sheet), and writes an OpenType font with TrueType outlines. It knows nothing about type design. It reads tables, transforms artwork into font units, and writes tables — nothing else.

---

## The Chain

```
ggwp --init    -->  manifest (.md) + blank glyph sheet (.svg)
                                        |
                                   Designer draws
                                        |
ggwp --build   -->  .ttf  -->  validated through FreeType
```

1. **Init** — create the manifest and a blank glyph sheet with reference lines and a cell grid
2. **Draw** — draw the glyphs in any SVG editor
3. **Build** — write the font from the manifest and the filled glyph sheet, then validate it

`ggwp --extract` runs the chain in reverse: it reads an existing font and writes a glyph sheet.

---

## Quick Start

```bash
# 1. Create the manifest and the regular weight's glyph sheet
ggwp --init="My Font"
#    -> my-font.md
#    -> sheets/GlyphSheet-Regular.svg

# 2. Open the sheet in your SVG editor and draw the glyphs

# 3. Build the font
ggwp --build=my-font.md --weight=regular --out=MyFont-Regular.ttf
```

---

## Command Line

Arguments select what runs. They never carry font rules.

| Command | What it does |
| --- | --- |
| `ggwp --build=<manifest.md> --weight=<table> --out=<font.ttf>` | read the manifest, read the weight's glyph sheet, write the font, then validate it |
| `ggwp --init=<font-name>` | create whatever is missing, then reconcile the glyph sheets against the manifest's glyph table |
| `ggwp --extract=<font.ttf> --manifest=<manifest.md> --weight=<table>` | read an existing font, then write that weight's glyph sheet |
| `ggwp --version` | the version number |
| `ggwp --help` | the full guide |

Each long option takes its value after an equals sign. A space does not work.

`--weight` names a table in the manifest, not a number. `--out` is relative to the current directory. The glyph sheet path is relative to the manifest's own directory.

---

## The Two Inputs

| Artifact | What it is |
| --- | --- |
| `<name>.md` | the manifest — metrics, names, and one table per weight |
| `<sheet>.svg` | the glyph sheet — one drawing per glyph |

One manifest declares every weight. One glyph sheet holds one weight.

The manifest is a markdown file of grid tables: `## metrics`, `## font`, `## glyphs`, `## guides`, `## colour`, and one table per weight. The `## glyphs` table is the single source of truth for which glyphs exist. `--init` reconciles every glyph sheet against it.

---

## The Glyph Sheet

Each glyph is a `<g>` element. Its `id` is an underscore and then one four-digit hexadecimal codepoint. More than one codepoint in one id makes a ligature.

```
_0041         the letter A
_2122         the trade mark sign
_0066_0069    the f-i ligature
```

The group holds one `<rect>`, which is the glyph's cell, and the artwork that follows it. The cell's position places the artwork. The cell's width sets the glyph's advance width.

A `<g id="guides">` layer holds each cell's label, its reference lines, and its cell guides. GGWP ignores this layer when it reads a sheet back.

| Reference line | Marks |
| --- | --- |
| `capHeight` | top of capital letters |
| `xHeight` | top of lowercase letters |
| `baseline` | where letters sit |
| `descender` | bottom of descenders (g, p, y) |

The `## guides` table places each line. The `## colour` table paints it.

---

## Determinism

Your manifest, your glyph sheet, and the binary determine the output bytes. No timestamps, no paths, no host state. A second run over the same inputs produces the same file.

Every failure is fatal and exits non-zero.

---

## Documentation

`ggwp --help` is the complete reference: the `--init` and `--extract` contracts, every manifest table and key, the glyph sheet rules, the tables GGWP writes, and the validation checks.

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

You don't need to understand typography jargon to make a font. You don't need to know what "UPM" or "fsSelection bit 7" means. You need to draw letters that look right, place reference lines where they feel right, and run one command. The eyes work. The vocabulary is just labels.

Total cost of a professional font pipeline: one binary.

---

## Installation

Download the installer from [Releases](https://github.com/jrengmusic/ggwp/releases).

| Platform | Architecture | Format |
| --- | --- | --- |
| macOS | universal | `.pkg` (signed, notarized) |
| Windows | x64 | `.exe` installer |
| Windows | arm64 | `.exe` installer |

- **macOS** — open the `.pkg`. It installs `ggwp` on your PATH.
- **Windows** — run the installer. It installs `ggwp.exe` and adds its folder to your user PATH.

---

## Build from Source

- C++17
- CMake 3.25 or later
- [JUCE](https://github.com/juce-framework/JUCE) 8.0.14, exact match, at `../../JUCE`
- JAM (JRENG Architectural Modules) at `../jam`

```bash
cmake -S . -B Builds
cmake --build Builds --config Release
```

---

## License

MIT — Use it, break it, fix it, ship it.

---
Rock 'n Roll!

**JRENG!**
---
conceived with [CAROL](https://github.com/jrengmusic/carol)
