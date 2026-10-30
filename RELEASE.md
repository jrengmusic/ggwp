# ggwp v1.0

GlyphSheet Generation Workflow Protocol

## What's New

- --extract writes a glyph sheet from a TTF, with ligatures found by shaping.
- The units per em is 1024 for every font. It is a contract constant, not a manifest value.
- A weight table may carry its own ascender, descender and lineGap rows, which replace the metrics for that weight.
- The post fixed pitch flag derives from the whole-cell advances of the glyphs.
- Cell widths are fractional, written to three decimal places.
- The colour table holds every colour of the sheet, and the guides table places the cell guides.
- The help is rendered with MarkdownComponent.
- The release conforms to cast and scout: a signed macOS pkg and Windows x64 and arm64 installers.

## Platforms

| Platform | Architecture       | Format                            |
| -------- | ------------------ | --------------------------------- |
| macOS    | universal          | .pkg (signed, notarized)          |
| Windows  | x64                | .exe installer                    |
| Windows  | arm64              | .exe installer                    |

## Installation

macOS: open the .pkg. It installs `ggwp` on your PATH.

Windows: run the installer. It installs `ggwp.exe` and adds its folder to your user PATH.
