# PLAN: Display Font Family on the ggwp Contract

**RFC:** none — objective from ARCHITECT prompt (session 2026-10-30)
**Date:** 2026-10-30
**BLESSED Compliance:** verified
**Language Constraints:** C++17 / JUCE / JAM (ggwp CLI, jam_opentype); font data: ggwp manifests (markdown) + SVG glyph sheets

## Overview

Step 0 fixes the ggwp contract and adds `--extract`. Then COUNSELOR pauses, and ARCHITECT builds and tests ggwp. The plan then builds a new font project with 4 families × 3 weights = 12 fonts, all on one contract (UPM 1024). The sheets are pure: base glyphs, ligatures, and custom PUA. There is no patching.

## ARCHITECT Rulings (verbatim)

| # | Ruling | Quote |
|---|---|---|
| R1 | ggwp is canon, no Python | "ggwp cli is now the CANON protocol to create fonts" · "NO python at all" |
| R2 | Extraction is Step 0 | "ok adding extraction features is STEP 0, fold into plan" · option verb `extract` (NAMES.md Rule 1/5, `--build` sibling) |
| R3 | Display sheets from ttf | "Display extract the glyphsheet from the ttf. it's more accurate." |
| R4 | UPM 1024 contract | "make 1024 the ggwp contract" · "make it consistent. all with 1024, update the HELP" · "DESIGN BY CONTRACT. all downstream only usage" |
| R5 | Normalise, no visual change | "why cant we normalise 1024 UPM == 1 em? then all glyphs scaled" · "there should not be any runtime usage visual changes" |
| R6 | Bold keeps its metrics | AskUserQuestion: "Weight table override" |
| R7 | Pure sheets, no patch | "glyphsheets should be PURE, base font + ligatures + custom PUA" · "no patch then." |
| R8 | `-->` on Mono | "add --> glyph to all weight to Display mono glyphsheet" |
| R9 | Mono ligatures to Display | "add all ligatures from Display mono to Display (jsut copy)" |
| R10 | Serif families | "Display Serif / Display Serif Mono … add the "serif" decoration … (probably only for letters and numbers) … symbols, special chars, ligatures etc, could stays as is" |
| R11 | Serif reference | "Engineer draw from reference. from those what geometrically close to our glyphs mathematically accurate? Right serif have nice example how a design could be both mono and serif" |
| R12 | Audit before rebuild | "rebuild all fonts from the glyphsheet, AFTER i audit the visual of new fonts" · "total should be 12" |
| R13 | Help page | "fix the help page - adding missing unclear info - render it using MarkdownComponent (in terminal graphics), look how cast and scout done it" |
| R14 | cast/scout conformance | "include ggwp conformance to cast/scout manifest so build release also publish package to github … SSOT ISOMORPHIC pattern for all jam CLI" |
| R15 | Build ownership | "you dont build. ARCHITECT will build. so you pause, when ggwp ready to extract. after ARCHITECT build and test runtime, then you continue the plan to completion" |
| R16 | Dataset home | "write new ggwp project repo. dev/fonts/display/ ..." |
| R17 | Fix the untested contract now | "i already fucking told you ggwp is untested. now is the time, step 0 fix the contract." (covers 0.3 fixed pitch and 0.5 fractional cells, each with its citation) |
| R18 | Rebuild every font | "rebuild all fonts from the glyphsheet" (the 6 runtime fonts live in `jam/resources/fonts`, embedded by `end/CMakeLists.txt:456-461` and `eve/CMakeLists.txt:510-518`) |
| R19 | Sheet colour and cell guides (Pause 2 audit) | "glyphs colour shouldnt be black. must be contrast to its dark background. also we need better better guide visualisation for showing CELL. with dashed lines at center, it's quite confusing to read 1 cell, and ligatures 2-3 cell," · "dont hack the glyphsheet, fix the producer" · "current colour scheme can be used as "dark" theme" · "NO HANDROLL anything allowed" |
| R20 | Colour lane | "just add to its md manifest. dedicated table ## colour" |
| R22 | Pause 2 serif correction | "1 is weird" · "G K Y X makes straight lines on top and bottom, GARBAGE" · "same issue with mono" · "dont just add the serif with straight lines, but with curve with narrow end." · "more toward like the following" (reference image: bold bracketed serif text) · "my point is dont get overboard, just need gentle serif accent" · "you can actually capture the the SVG as image and validate the visual no?" · "go" on: bracketed tapered accents (PP Right Serif Mono bracket profile, scaled to a gentle accent); G spur, beaks and the flat bars on X, Y and K removed; diagonal ends accented only where the bracket can sit on the inner side; `1` keeps its own flag with a small bracketed foot; every sheet rendered with rsvg-convert and visually checked |
| R23 | Serifs descoped to ARCHITECT | "still ugly and this not working. i will find another way to design the serifs. just copy the Display/Display Mono glyphsheet as starting point." |
| R24 | Endpoint | "[] reset" · "[] no need build. i will build all when serief ready" · "[] /audit - doxygen - until /log (for ggwp and jam)" — Step 7 is ARCHITECT's; the sprint ends with the audit, the doxygen pass and the log |
| R21 | Fixed pitch in cells | "/go" on: fixed pitch when every single-codepoint advance is a whole multiple of the plain advance |

## Step 5b: Sheet colour table and cell guides (R19, R20)

**Scope:** ggwp `Source/Manifest.{h,cpp}`, `Guides.{h,cpp}`, `Sheet.cpp`, `Init.cpp`, `HELP.md`, `cast/identifiers.md`; then the four `dev/fonts/*` manifests and sheets, regenerated by ggwp only.

**Action:**
1. A new manifest table `## colour` (columns `key`, `colour`) holds every sheet colour. The current colours are the defaults ("dark"). The glyph colour defaults to `#E0E0E0`, the artwork colour of the original sheets (`ggwp/display-serif/sheets/GlyphSheet-Book.svg`: `fill="rgb(224,224,224)"`).
2. `## guides` keeps `key` and `offset`. Its line colour is read from `## colour` by the guide's key (one source).
3. The producer reads every colour from `## colour`: the background, the cell rect stroke, the label, the guide lines, the glyph fill (on the glyph `<g>`), and the cell guides.
4. The dashed centreline is replaced by cell guides at the true advance in sheet space. These are the unit-cell edges at k × plainAdvance ÷ sheetScale, and the advance end, from `OpenTypeDocument::getPlainAdvance`, `getSheetScale` and `getMetricUnit`.
5. `--init` adds a missing `## colour` table, as it does for `## guides`.
6. HELP.md documents the table and the cell guides.
7. ARCHITECT runs cast and the build. Then the four font manifests regenerate their `## guides` and `## colour` tables with `--init`, and every built font must be byte-identical to its build before the change.

## Language / Framework Constraints

- LANGUAGE.md C++/JUCE: one object is one header and one `.cpp`. New ggwp unit `Extract.h/.cpp` mirrors `Init.h/.cpp`.
- CODING.md: no bail-out guards, `not/and/or`, brace init, no out-params, no fake-carrier structs, `.at()`, no `==` on strings.
- Submodule files have no includes. jam_opentype headers start with `#pragma once` and `namespace jam` (`jam_OpenTypeDocument.h:1-4`).
- Agents run no cmake/ninja. Engineer runs the `ggwp` binary only after Pause 1.

## Dependency & API Inventory

- **FreeType** (already linked in ggwp, `Main.cpp:1-2`; `jam_graphics.h:56-60`). Reads cmap, glyph outlines and advances.
- **HarfBuzz** (`jam_opentype.h:37-38`; include row `project-info.md` harfbuzz). Reads GSUB ligature lookups. The exact API comes from Librarian (Step 0.4a).
- **jam::Svg::Format::pathToString** (`jam_Svg.h:813`, `jam_SvgFormat.cpp:19-51`). Turns a path into SVG `d` data. ggwp already calls it (`Artwork.cpp:40`).
- **jam::OpenTypeDocument**:
  - `getGlyphTransform` (private, `jam_OpenTypeDocument.h:374`; body `jam_OpenTypeDocumentGlyph.cpp:5-14`). The sheet-to-font transform. Extraction uses its `juce::AffineTransform::inverted()`.
  - `getGlyphGroups`, `getCodepoints` and `getGlyphId` (`:81-98`).
  - `getMetricUnit` (`jam_OpenTypeDocument.cpp:61-74`).
- **ggwp reuse:**
  - `getSheetSvg` lays out and repacks (`Sheet.h:25`).
  - `getSheetDocument` (`Sheet.h:38`).
  - `addGlyphs` builds the glyphs table from a sheet root (`Manifest.h:57`).
  - `getWeightSheetFile` (`Manifest.h:22`).
  - `writeManifestFile` / `writeSheetFile` pattern (`Init.cpp:7-24`).
  - `getArtworkSvg` shift table (`Artwork.cpp:30-52`).
- **Identifiers:** `Id::unitsPerEm`, `Id::isFixedPitch`, `Id::ascender`, `Id::descender`, `Id::lineGap` exist (`jam/cast/opentype.md:28-62`). `Id::baseline` and `Id::cellWidth` are in `jam/cast/core.md:51,54`.
- **Help render:** the scout pattern is copied as is (`scout/Source/Help.h`, `Help.cpp:3-35`; `scout/Source/Main.cpp:137-145` GUI initialiser). The stylesheets are `scout/Source/markdown.css` and `jam/resources/mermaid.css` (`scout/project-info.md:186-194`).
- **cast archetype:** `dev/archetype/cli/cast/{spell.md,cmake.cast,installer.cast,signing.md}`. scout consumes it (`scout/cast/spell.md:6-31`, `:146-228`).

## Measured Source Facts (feed the manifests)

| Font | UPM | asc / desc / gap | H top | x top | Source |
|---|---|---|---|---|---|
| Display Book, Medium | 1024 | 881 / −122 / 0 | 740 | 550 / 549 | `jam/resources/fonts` (hb-info, hb-shape) |
| Display Bold | 1024 | 829 / −114 / 0 | 740 | 558 | same |
| Display Mono Book/Med/Bold | 1003 | 881 / −122 / 250 | 745 / 746 / 743 | 534 / 534 / 540 | `end/design/fonts/Display Mono` (built from `svg-mono`, `build_fonts.py:44-45`) |

Mono in 1024 units (× 1024/1003): asc 899, desc −125, gap 255, H top 761, x top 545. Advance 615 → 628.

## Validation Gate

COUNSELOR validates each step before the next one. Validation covers MANIFESTO.md (BLESSED), NAMES.md, ~/.carol/CODING.md, and the locked PLAN decisions: no deviation and no scope drift. @Auditor runs ONCE, after the final step, over the whole sprint. It never runs per step.

## Steps

### Step 0.1: UPM 1024 constant (jam_opentype)
**Scope:** `jam/jam_opentype/document/jam_OpenTypeDocument.h`, `jam_OpenTypeDocumentTables.cpp`
**Action:**
1. Add `static constexpr int unitsPerEm { 1024 };` beside `spaceCodepoint` (`.h:31`).
2. `addHeadTable` writes `unitsPerEm` (`Tables.cpp:51`). Delete the `ascender - descender` derivation and its two reads (`:46-47`).
**Validation:** no other UPM source remains. The constant sits in the narrowest scope that holds both users (head writer and extraction).

### Step 0.2: Weight-table metric override (jam_opentype)
**Scope:** `jam_OpenTypeDocumentTables.cpp`, `jam_OpenTypeDocument.h` (private decl)
**Action:**
1. Add `static juce::ValueTree getWeightMetrics (const juce::ValueTree& values, const juce::Identifier& weightId);`. It returns the family `metrics` tree, with the weight table's own `ascender`, `descender` and `lineGap` set where the weight table carries them. This is a creation-phase copy.
2. `addTables` (`:110-128`) passes this tree to `addHheaTable`, `addOs2Table` and `addGlyfTable` instead of the raw family `metrics` (`:113`).
**Validation:** R6. Bold builds 829/−114/0 while Book and Medium build 881/−122/0. No new identifier beyond `getWeightMetrics` (a `get*` family member).

### Step 0.3: Fixed pitch from data (jam_opentype)
**Scope:** `jam_OpenTypeDocument.h:119,125`, the post and OS/2 PANOSE writers
**Action:**
1. Replace the constants `fixedPitch { 1 }` (`:125`, doc "this font family is monospaced") and `panoseProportion { 9 }` (`:119`).
2. Add `static bool isFixedPitch (const Element& glyf)`. It is true when every single-codepoint glyph with a non-zero advance has the same advance.
3. post `isFixedPitch` and PANOSE bProportion read it. Use 9 (monospaced) when it is true. Add a named constant for the proportional PANOSE value.
**Validation:** Display, which is proportional (`fc-query` spacing empty), no longer reports monospace. Mono still does.
**As implemented (R21):** the same-advance rule in item 2 reported Mono as proportional, because Mono's E000 and E001 are 2 cells wide. Per R21, `isFixedPitch (const Element& glyf, int plainAdvance)` is true when every single-codepoint advance is a whole multiple of the plain advance (`jam_OpenTypeDocumentTables.cpp:66-77`). Verified: post.isFixedPitch is 1 for the six Mono fonts and 0 for the six proportional fonts.

### Step 0.4: `--extract` (ggwp)
**Scope:** new `ggwp/Source/Extract.h/.cpp`; `Main.cpp` registration; `jam_OpenTypeDocument.h` (`getGlyphTransform` moves from private to public, `:374`)
**0.4a:** @Librarian (library-mode) researches two things:
- The HarfBuzz API that lists GSUB LookupType 4 ligatures (input glyph sequence → ligature glyph) from an `hb_face_t`.
- The FreeType calls for the cmap walk (`FT_Get_First_Char`/`FT_Get_Next_Char`), unscaled loading (`FT_LOAD_NO_SCALE`) and `FT_Outline_Decompose`, including the CFF cubic case.

The findings go at the top of the Engineer prompt.
**As implemented:** the ligatures are found by shaping. Extraction collects every glyph that a GSUB lookup takes as input, shapes each sequence of two and of three such characters, and takes a sequence that shapes to one glyph as a ligature (`Extract.cpp:118-144`, `HELP.md:52`). It reads no LookupType 4 table directly.
**Action:**
1. CLI: `ggwp --extract=<font.ttf> --manifest=<manifest.md> --weight=<table>`. `extractOption` and `runExtractCommand` mirror `initOption`/`runInitCommand` (`Init.h:12,29`).
2. Read the manifest, which must exist and pass `jam::ConfigValidator` (`Init.cpp:125-128`). The metrics are read-only (R4 "downstream only usage").
3. Walk the cmap. Skip U+0000 and U+0020 (reserved, `Sheet.cpp:97-104`).
   **As implemented:** the skip covers every codepoint at or below U+0020 (`Extract.cpp:178`, `HELP.md:50`). The first Display extraction wrote the source's U+000D mapping as a manifest row, and the CR split the row; the Medium and Bold round trips then failed. A control character cannot be carried in a manifest text cell.
4. For each glyph, scale the outline by `unitsPerEm / face->units_per_EM` (R5). Map it into sheet space with `getGlyphTransform (scale, baseline, cellX, cellY).inverted()`. Write it as one `<path d=…>` with `Svg::Format::pathToString`.
5. Cell width = scaled advance ÷ family advance × `cellWidth`, kept fractional (see 0.5).
6. Ligatures: each GSUB ligature becomes a multi-codepoint cell. Its width is `getLigatureCellWidth` (`Sheet.cpp:55-58`), because the build ignores ligature width (HELP).
7. Build a raw sheet text from these groups. If the manifest has no `## glyphs` table, call `addGlyphs (manifest, rawRoot)` and write the manifest (`Init.cpp:26-60` pattern).
8. Write the weight sheet as `getSheetSvg (manifest, rawSheetText)` to `getWeightSheetFile` (`Init.cpp:78-93` pattern).
**Validation:**
- No hand-rolled transform: only the inverse of the existing one.
- No Python.
- Names are verbatim from this plan: `extractOption`, `runExtractCommand`, `--manifest`.
- `getGlyphTransform` becomes public only because extraction calls it (proven caller).

### Step 0.5: Fractional cell geometry (ggwp)
**Scope:** `ggwp/Source/Sheet.cpp:130-136,138-163,215-273`, `Artwork.h/.cpp`
**Action:**
1. Cell x and width become `float`, written with 3 decimals. Reuse the `formatCoordinate` rule (`Artwork.cpp:8-13`).
2. `getCellWidth` reads `getFloatValue`.
3. The `getArtworkSvg` delta and the `Function::Map` registration change from `int, int` to `float, float`. Pass the values as prvalues (sprint-log crash, `jam_Function.h:44-45,58-61`).
4. The build already reads double widths (`jam_OpenTypeDocumentGlyph.cpp:176,186`).
**Validation:** extract-then-build reproduces each source advance exactly. Without this, integer cells round by up to 5 font units, which breaks R5.

### Step 0.6: Help render and HELP.md (ggwp)
**Scope:** `Source/Help.h` → `Help.h/.cpp` (scout copy); new `Source/markdown.css` (copy of `scout/Source/markdown.css`); `Main.cpp` (GUI initialiser scope and banner, as in `scout/Source/Main.cpp:18-23,137-145`); `Source/HELP.md`
**Action:**
1. Port `printHelp` verbatim, with `files::ggwpHelp` in place of `files::scoutHelp` (`scout Help.cpp:19`).
2. HELP.md moves its ANSI banner (lines 1-12) to `cast/banner.md`.
3. HELP.md documents:
   - UPM 1024 as a contract.
   - Metrics in that UPM, with the weight-table `ascender`/`descender`/`lineGap` override.
   - `--extract` and its normalisation.
   - Fractional cell geometry.
   - Fixed pitch derived from the advances.
   - An empty cell produces no glyph (`jam_OpenTypeDocumentGlyph.cpp:181`).
   - Ligature matching is longest first (`jam_OpenTypeDocument.h:276-281`).
   - `--weight` matching (`Main.cpp:121-128`).
**Validation:** the help renders through `jam::MarkdownComponent` and `TerminalBuffer` exactly as scout does. HELP.md states nothing the code does not do.

### Step 0.7: cast/scout conformance (ggwp)
**Scope:** `ggwp/cast/{spell.md, identifiers.md, text.md, banner.md, files.md}`, `ggwp/project-info.md`; delete `ggwp/cast/cmake.cast` and `ggwp/cast/installer.cast` (replaced by the archetype)
**Action:**
1. `spell.md` takes scout's shape (`scout/cast/spell.md:1-228`): `@cmake`/`@installer` → `../../archetype/cli/cast/*.cast`, plus the Identifiers/Text/Files/HashMaps/Generated outputs.
2. `project-info.md`:
   - adds the `## binary` rows help/markdownStyleSheet/mermaidStyleSheet (`scout project-info.md:186-194`);
   - adds the user modules needed for help render (`:129-143`, keeping jam_opentype);
   - adds the freetype include row (`:182`);
   - adds the toolchain `cast ../jam/cast/spell.md` rows (`:198-220`).
3. `gh.cmake` and `RELEASE.md` stay generated (`@Gh`, `@Release`).
**Validation:** `diff` of ggwp `spell.md` and `project-info.md` against scout shows only project-specific values. ARCHITECT runs `cast` and the build.

### Step 0.8: PAUSE 1 — ARCHITECT builds and tests ggwp (R15)
COUNSELOR stops. ARCHITECT runs cast, builds and tests at runtime. The plan resumes on ARCHITECT's word.

### Step 1: Font project skeleton (R16)
**Scope:** new `~/Documents/Poems/dev/fonts/{display,display-mono,display-serif,display-serif-mono}/` (git init is ARCHITECT's)
**Action:**
1. In each directory, run `ggwp --init="<Family>"`, which writes `<family>.md` (`HELP.md`; `Init.cpp:102-123`).
2. Edit each manifest:
   - `## metrics` from the Measured Source Facts. Display family: asc 881, desc −122, gap 0, capHeight 740, xHeight 550. Mono family: 899 / −125 / 255, capHeight 761, xHeight 545. `advanceWidthFactor` gives a 628 advance at `cellWidth` 120. Display uses the same factor, so copied ligatures keep the Mono width.
   - The Display Bold weight table carries `ascender 829`, `descender -114` (R6).
   - `## font` strings from `display-serif.md:24-31`.
   - Weight tables `book`/`medium`/`bold` replace `regular`. Styles Book/Medium/Bold, weightClass 350/500/700 (`sheet_config.py:202-216`). PostScript `Display-Book`, `DisplayMono-Book`, `DisplaySerif-Book`, `DisplaySerifMono-Book` and so on (`display-serif.md:42` family).
**Validation:** `jam::ConfigValidator` passes. Values trace to the cited measurements.

### Step 2: Extract Display and Display Mono (R3, R5)
**Action:** run `ggwp --extract` for each of the 6 sources:
- Display: `jam/resources/fonts/Display{Book,Medium,Bold}.ttf`.
- Display Mono: `end/design/fonts/Display Mono/DisplayMono-{Book,Medium,Bold}.ttf`.

Then run `ggwp --build` per weight to a scratch dir. Compare `hb-shape --show-extents` on `HxnopdlIO0` and the ligatures against the source, after the ×1024/UPM scale.
**Validation:** advances and extents match the source scaled to 1024, within 1 unit. Ligatures and E000/E001 are present.

### Step 3: `-->` on Display Mono (R8)
**Action:**
1. Add the `` `-->` `` row to the display-mono glyphs table, then run `--init`.
2. In each Mono sheet, Engineer composes the `-->` artwork from that weight's `->` cell: the same head, with the shaft extended by one cell (`cellWidth` + gutter).
**Validation:** shaping `-->` gives one glyph at advance 3 × 628.

### Step 4: Mono ligatures into Display (R9)
**Action:**
1. Add every display-mono ligature row, including `-->`, to the display glyphs table, then run `--init`.
2. Per weight, replace each empty Display ligature group with the Mono group verbatim (rect and artwork).
3. Run `--init` again, which shifts the artwork into the Display cell (`Sheet.cpp:148-158`).

The PUA is not copied (not in R9).
**Validation:** Display shapes every Mono ligature to one glyph.

### Step 5: Serif and Serif Mono sheets (R10, R11)
**Action:**
1. Make display-serif a copy of the display sheets and glyphs table. Make display-serif-mono a copy of display-mono.
2. For A–Z, a–z and 0–9 only, Engineer adds serif slabs as `<rect>` elements in each cell. Overlapping contours fill under the nonzero rule.
3. Geometry comes from IBM Plex Serif Regular, which measured closest:
   - x-height ÷ cap: 0.739 against our 0.714. Stem ÷ cap: 0.123 against our 0.099.
   - Its slabs are straight-line rectangles (`hb-view` paths, `M`/`L` only).
   - Slab thickness = 46/86 × the glyph's own stem. Overhang per side = 75/86 × stem. Both measured on the Plex `H`.
   - Slab placement per glyph follows the Plex glyph of the same codepoint. Engineer extracts the Plex outlines with `hb-view --output-format=svg`.
4. Serif Mono: narrow letters (`i j l r f t I 1`) follow PP Right Serif Mono's cell-filling slab placement, with its `l` tail as the model.
5. Symbols, punctuation, ligatures and PUA stay unchanged.
**Validation:** every slab dimension traces to the ratios above and the glyph's measured stem. No glyph outside A–Z/a–z/0–9 changes.

### Step 6: PAUSE 2 — ARCHITECT visual audit (R12)
COUNSELOR stops. ARCHITECT audits the Serif, Serif Mono, `-->` and Display ligature artwork. The plan resumes on ARCHITECT's word, with corrections applied first.

### Step 7: Build all 12 fonts (R12)
**Action:**
1. Run `ggwp --build` for each family and weight into `dev/fonts/<family>/fonts/`.
2. Copy the 6 rebuilt Display and Display Mono fonts over `jam/resources/fonts/Display{Book,Medium,Bold}.ttf` and `DisplayMono{Book,Medium,Bold}.ttf`. The file names stay the same (`EventRegistration.cpp:31-36`, eve `style.css`).
**Validation:** 12 fonts. Every font reports UPM 1024 (hb-info). The ggwp round-trip validation passes (`Main.cpp:89-103`).

### Step 8: Audit
@Auditor sweeps the whole sprint: ggwp, jam_opentype and the font project. COUNSELOR resolves every validated finding.

## BLESSED Alignment

- **B:** each new object has one owner. `Extract.h/.cpp` is one unit that owns its FreeType face for one run.
- **L:** `runExtractCommand` splits by responsibility: cmap walk, glyph conversion, ligature read, write. It reuses `getSheetSvg`, so there is no second layout.
- **E:** no bail-outs. Fail-fast uses `juce::ConsoleApplication::fail`, as the siblings do.
- **S (SSOT):**
  - UPM has one constant.
  - The manifest is the contract, and extract and build only read it (R4).
  - Layout stays in `getSheetSvg`.
  - The transform has one source (`getGlyphTransform`) and its inverse.
- **S (Stateless):** `Extract` holds no state across calls.
- **E (Encapsulation):** `getGlyphTransform` becomes public for one proven caller. No friend.
- **D:** extract and build are deterministic. ggwp's byte-identity rule holds (HELP "Determinism").

## Risks / Open Questions

None. All points are closed by R1–R18.
