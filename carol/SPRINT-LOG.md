# SPRINT-LOG.md

**Project:** ggwp  
**Repository:** /Users/jreng/Documents/Poems/dev/ggwp  
**Started:** 2026-05-01

**Purpose:** Long-term context memory across sessions. Tracks completed work, debts paid, and debts deferred to the inter-sprint ledger (`DEBT.md`). Written by PRIMARY agents only when ARCHITECT explicitly requests.

---

## 📖 Notation Reference

**[N]** = Sprint Number (e.g., `1`, `2`, `3`...)

**Sprint:** A discrete unit of work completed by one or more agents, ending with ARCHITECT approval ("done", "good", "commit")

---

## ⚠️ CRITICAL RULES

**AGENTS BUILD CODE FOR ARCHITECT TO TEST**
- Agents build/modify code ONLY when ARCHITECT explicitly requests
- ARCHITECT tests and provides feedback
- Agents wait for ARCHITECT approval before proceeding

**AGENTS NEVER RUN GIT COMMANDS**
- Write code changes without running git commands
- Agent runs git ONLY when user explicitly requests
- Never autonomous git operations
- **When committing:** Always stage ALL changes with `git add -A` before commit
  - ❌ DON'T selectively stage files (agents forget/miss files)
  - ✅ DO `git add -A` to capture every modified file

**SPRINT-LOG WRITTEN BY PRIMARY AGENTS ONLY**
- **COUNSELOR** or **SURGEON** write to SPRINT-LOG
- Only when user explicitly says: `"log sprint"`
- No intermediate summary files
- No automatic logging after every task
- Latest sprint at top, keep last 5 entries

**NAMING RULE (CODE VOCABULARY)**
- All identifiers must obey project-specific naming conventions (see NAMES.md)
- Variable names: semantic + precise (not `temp`, `data`, `x`)
- Function names: verb-noun pattern (initRepository, detectCanonBranch)
- Struct fields: domain-specific terminology (not generic `value`, `item`, `entry`)
- Type names: PascalCase, clear intent (CanonBranchConfig, not BranchData)

**BEFORE CODING: ALWAYS SEARCH EXISTING PATTERNS**
- ❌ NEVER invent new states, enums, or utility functions without checking if they exist
- ✅ Always grep/search the codebase first for existing patterns
- ✅ Check types, constants, and error handling patterns before creating new ones
- **Methodology:** Read → Understand → Find SSOT → Use existing pattern

**TRUST THE LIBRARY, DON'T REINVENT**
- ❌ NEVER create custom helpers for things the library/framework already does
- ✅ Trust the library/framework - it's battle-tested

**FAIL-FAST RULE (CRITICAL)**
- ❌ NEVER silently ignore errors (no error suppression)
- ❌ NEVER use fallback values that mask failures
- ❌ NEVER return empty strings/zero values when operations fail
- ❌ NEVER use early returns
- ✅ ALWAYS check error returns explicitly
- ✅ ALWAYS return errors to caller or log + fail fast

**⚠️ NEVER REMOVE THESE RULES**
- Rules at top of SPRINT-LOG.md are immutable
- If rules need update: ADD new rules, don't erase old ones

---

## Quick Reference

### For Agents

**When user says:** `"log sprint"`

1. **Check:** Did I (PRIMARY agent) complete work this session?
2. **If YES:** Write sprint block to SPRINT-LOG.md (latest first)
3. **Include:** Files modified, changes made, alignment check, debts paid, debts deferred
4. **Hygiene:** After writing the SPRINT-LOG entry, drain paid debts from project-root `DEBT.md` via `carol debt clear <id>` for each ID under *Debts Paid*. Receipt first, then clean the books.

### For User

**Activate PRIMARY:**
```
"@CAROL.md COUNSELOR: Rock 'n Roll"
"@CAROL.md SURGEON: Rock 'n Roll"
```

**Log completed work:**
```
"log sprint"
```

**Invoke subagent:**
```
"@oracle analyze this"
"@engineer scaffold that"
"@auditor verify this"
```

**Available Agents:**
- **PRIMARY:** COUNSELOR (domain specific strategic analysis), SURGEON (surgical precision problem solving)
- **Subagents:** Pathfinder, Oracle, Engineer, Auditor, Machinist, Librarian

---

<!-- SPRINT HISTORY STARTS BELOW -->
<!-- Latest sprint at top, oldest at bottom -->
<!-- Keep last 5 sprints, rotate older to git history -->

## SPRINT HISTORY

## Sprint 5: Const-Only Reader Law, Forward-Decl Purge, Full Audit Sweep ✅

**Date:** 2026-09-17
**Duration:** ~08:00 (session spanned two calendar days)

### Agents Participated
- COUNSELOR (fable-5) — planning, per-step validation, audit triage, trivial fixes (Document begin/end dual, CODING.md rule), sprint log
- Pathfinder — sprint-state discovery, on-disk const-refactor inventory, byte-identity/idempotence verification (×2, pre-const-law)
- Engineer (×7 waves) — forward-decl purge + svg split, jam_core const surface, jam_markdown const-only family, mermaid parent-const threading, cast const rewrite (×2), consumer sweep, audit fixes (×2)
- Auditor (×2) — mid-session comprehensive audit (~70 findings), final const-law audit (32 findings)

### Files Modified (~90 total; representative)
**jam — const-only reader law (this sprint's core):**
- `jam_core/document/jam_Document.h` — Element::Iterator templated on ElementType (const range-for yields `const Element*`); `Element::get<T>`/`getChildByID`/`Token::get` collapsed to single static deduced-constness template bodies (zero const_cast); `Element::parent` → `const Element*`; `root` protected behind dual `getRoot()`; new `Document::getChildByID (const Element&, Identifier)` O(1) keyed-probe SSOT; Index ctors retyped `Document&` (live-registry lane is mutable by ruling)
- `jam_markdown/document/jam_MarkdownDocument.{h,cpp}` — all 16 non-const reader overloads DELETED; const overloads are the single bodies (16 const_casts died); `isTable` predicate (sibling of isTableBorder); provenance stamping rewritten as structural creation walk; 7 duplicated probe bodies → one `getChildByID` call each; law doxygen updated
- `jam_markdown/document/jam_MarkdownWriter.{h,cpp}` — const element arrays; String `==` → `compare() == 0`
- `jam_mermaid_diagram/diagram/*` — upward parent-climb results retyped const identities; downward enumerations take mutable level; 3-arg `getLevelVertex` overload (mutable-twin resolution by address-descent); null guards; enumeration hoisted out of edge loop
- `jam_opentype/document/*` — getGlyphGroups/getLigatureGroup/getOrderedLigatureGroup/getTable retyped const
- 54 files across jam modules — `.root`/`->root` → `getRoot()` (237 call sites), const threaded per holder
**cast:**
- `Source/{Model,Validator,Writer,Shapes,Items,TemplateDocument}.h` — 14 const_casts removed (13 inventoried + 1 found in Items.h); creation lane navigates structurally (`*document.root` walks, `row.getChildByID`); read-only surface const end-to-end; Writer/TemplateDocument root access via getRoot()
**ggwp:**
- `Source/{Manifest.cpp,Init.cpp,Sheet.cpp}` — getRoot() migration (8 sites); Sheet.cpp:207 move-only const-local fix
**Contract:**
- `~/.carol/CODING.md` — "Readers return const" tightened: reader API const-only; creation/state-update code reaches elements through creation API + structural navigation, never by re-finding through readers
- `PLAN-const-only-readers.md` — locked plan (kept until ARCHITECT builds green)

**Earlier this session (pre-const-law, same sprint):** jam_graphics svg 7-file split (all <300 lines), forward declarations eliminated framework-wide, jam_ConfigDocument/ConfigValidator empty-value contract (value lane declares, empty non-string rejected via text::English::failEmptyValue), row-keying unified, informative getValueTree throw, jam_XML getIndent public, END cast-data convergence (source glob, @source alias, cellWidth from jam), END/eve/ggwp built clean at that checkpoint, byte-identity + --init idempotence verified twice (DisplaySerif-{Book,Medium,Bold}.ttf = 34128/34160/32596, hashes identical).

### Alignment Check
- [x] BLESSED principles followed
- [x] NAMES.md adhered — new names this sprint: `isTable`, `getRoot`, `TokenType`, `ElementType` usage, `getChildByID`/`getLevelVertex` overloads (family reuse)
- [x] MANIFESTO.md principles applied — creation-window mutation via mutable chains only; two-tree copy rejected on span-source evidence (jam_Document.h:816-820, :870-887)

### Problems Solved
- **Option B ruled and landed:** reader API is const-only, absolute; a `const` document yields no mutable interior access via readers, iteration, or `root`. Zero const_cast across jam_core/document, jam_markdown, jam_mermaid_diagram, jam_opentype, cast/Source, ggwp/Source (30 casts removed sprint-wide).
- Mermaid upward-climb vs creation-mutation conflict solved lawfully: const identities for reads; 3-arg getLevelVertex resolves the mutable twin by exact parent-chain address descent (flat enumeration proven unsound at nesting ≥2 and rejected).
- cast's enrichment stays multi-pass creation (MANIFESTO E) but never re-finds through readers; Items.h's stale mutable typing (root cause of 5 residual casts) retyped const.
- Audit (32 findings): 7 compile breaks fixed, law leaks closed (Model getNextLine/getNextShapeLine, OpenType getTable, MarkdownWriter, MermaidGit), SSOT probe collapse ×7, null-deref guards, brace-init/string-compare style, doxygen truth pass (new members documented, 5 stale root references corrected).
- Rejected with citations: deduced-constness remedy for getLevelVertex (refuted by `parent` constness, jam_Document.h:208); duplicate ancestor-walk (SSOT threshold is >2, found 2); assert-then-deref (assert-at-boundary per MANIFESTO D).

### State for Continuation
- **ARCHITECT builds next:** cast, ggwp, END, eve, whatdbg — compiler is the final verifier of the sweep; `Index (Document&)` retype enumerates END terminal callers at build. Then: cast regen byte-check, ggwp byte-identity + idempotence re-verify against refs above, jam → KANJUT `cast --sync` + jfs rebuild, doxygen regen zero-warning check.
- **Design residuals (Auditor, verbatim — ARCHITECT dispositions):**
  - `Element::firstChild/lastChild/nextSibling` remain public mutable (jam_Document.h:209-211) — a `const Element&` still yields mutable children by field access; the law holds by API discipline there, not compiler.
  - `Element*`/`Elements` inside `Document::Value` (jam_Document.h:181-182) — property-stored pointers bypass constness (live in mermaid model).
  - Model.h:27 `using jam::MarkdownDocument::getTableHeaderRow` republishes a protected base reader for Items.h:857 (friend-equivalent bypass).
  - Validator.h:469 `isPlaceholderScope` returns `std::pair<Result, const Element*>` — `is` verb contract violation (pre-existing shape).
  - Mermaid `addLayerOrder`/`addClusterEdgeEndpoints` exceed 30-line/branch thresholds (pre-existing base + sprint additions).
  - Reader family returns freshly built `jam::Array<const Element*>` per call (pre-existing API shape, preserved).
  - Container-internal const_casts outside named scope: jam_Owner.h:70,:251 (plan-listed), jam_HashMap.h:2069/2094/3120, jam_SharedResources.h:64, jam_Buffer.h:462, jam_Block.h:65/83, jam_ColourScheme.h, jam_TerminalTTY_posix.cpp, jam_MenuWindows.cpp.
- Delete `PLAN-const-only-readers.md` once ARCHITECT confirms green builds.

### Debts Paid
- None

### Debts Deferred
- None

---

## Sprint 4: Idempotent Table-Driven `--init` ✅

**Date:** 2026-09-16

### Objective

ARCHITECT's specification, verbatim:

```
how about 2 step check.
- table and glyphsheet
- table always SSOT, unidirectional to glyphsheet. with the exception of generation.
- table and glyphsheet doesnt exists? generate both. table default, glyphsheet empty.
- table exists? no glyphshet? create empty glyphsheet
- glyphsheet exists but no table? generate table from existing glyphsheet.
- when both exists, table is the SSOT. addition or deletion to table always affect glyphsheet.
```

Four constraints: no new arguments, destroy no glyph that exists on the table, re-layout
the canvas in both directions, and make the design universal.

### Agents Participated

- **COUNSELOR:** fable-5 — Directed the sprint. Validated every landing by direct read.
- **ENGINEER** (invoked by COUNSELOR) — All implementation, thirteen delegations
- **PATHFINDER** (invoked by COUNSELOR) — Reproduction runs, byte measurement, crash capture
- **LIBRARIAN** (invoked by COUNSELOR) — Two library-mode passes over the JUCE and jam path surfaces
- **AUDITOR** (invoked by COUNSELOR) — Two sweeps, the second over all fifteen sprint artifacts

### Result

All four states work, verified against the real `display-serif` dataset rather than
against the case where nothing moves.

```
Book   34128 bytes  100 glyphs
Medium 34160 bytes  100 glyphs
Bold   32596 bytes  100 glyphs
```

Re-measured after every batch. Second and third `--init` byte-identical across all three
sheets. Blank project writes 851 lines and 94 glyph groups.

Removing a glyph row repacks the sheet: `$` moved from `x="310"` to `x="186"`, and its
artwork offset from its own cell stayed `(91.63, 138.92)`. Adding a row creates an empty
cell at the manifest's `cellWidth`, and the font is byte-identical because an empty cell
carries no outline.

### Files Modified

**ggwp**
- `Source/Init.h` / `Source/Init.cpp` — four-state dispatch, `getSheetDocument`,
  `addMissingGlyphsTable`, `addMissingGuidesTable`, `writeWeightSheets`
- `Source/Manifest.h` / `Source/Manifest.cpp` — new unit; manifest construction moved out
  of `Init.cpp`; `## guides` table; sheet-seeded `## glyphs` table; `getCellText`,
  `getGlyphText`
- `Source/Sheet.h` / `Source/Sheet.cpp` — new unit; one-pass reconcile and emitter
- `Source/Main.cpp` — `--init` description strings, option constants scoped to their users
- `Source/HELP.md` — six factual errors corrected, `## glyphs` and `## guides` documented,
  the `--init` contract, the id grammar, the reserved-glyph rule

**jam**
- `jam_core/xml/jam_XML.h` — XML emitter, `toDocument (const juce::ValueTree&)`
- `jam_core/text/jam_Format.h` — `escape` made reachable
- `jam_graphics/svg/jam_SVG.cpp` — bail-out guards removed, brace initialisation, dead
  replaces removed, single `path.toString()` evaluation
- `jam_graphics/svg/jam_Svg.h` — `pathData` declaration removed
- `jam_opentype/document/jam_OpenTypeDocument.h` / `.cpp` — `getGlyphGroups`;
  `getCodepoints` now requires the leading underscore `getGlyphId` writes
- `jam_opentype/document/jam_OpenTypeDocumentGlyph.cpp` — filter and sort extracted
- `cast/identifiers.md` — `guides`, `xmlns`

### Problems Solved

**`--init` segfaulted whenever a cell moved.** `EXC_BAD_ACCESS` at `0x20` inside
`getSheetSvg`. Root cause was a `jam::Function::Map` deduction mismatch: the registration
declared `add<const Element&, int, int>` while the call site passed `deltaX` and `deltaY`
as named lvalues, which deduce `int&`. `jam_Function.h:44-45` states the rule, and
`:58-61` states that a mismatch cannot be caught by the compiler and crashes at the call
site in a release build. Fixed by passing both as prvalues. The arm had never executed
before, because until this sprint no test moved a cell.

**The background rectangle painted over the whole sheet from the second run onward.**
The parsed background rect fell through to passthrough, which was emitted after the cells.
Invisible to every test: font bytes do not depend on paint order, and idempotence held
because the order was consistently wrong.

**Artwork was destroyed by a case-variant id.** Cell matching compared id text, while
`getGlyphId` always writes uppercase padded hex and `getCodepoints` accepted any case.
A lowercase id decoded as a glyph, so it was excluded from passthrough, but never matched,
so an empty cell replaced the drawing. Matching now compares decoded codepoints.

**A designer's own `<g id="cafe">` was deleted.** `getCodepoints` accepted ids the encoder
can never produce. It now requires the leading underscore.

**A computed shift was silently discarded.** The attribute override map could replace a
key but never add one, so an element without an explicit `x` kept its position while its
cell moved. The emitter now emits an override key the element lacks.

**`pathData` duplicated `pathToString`.** COUNSELOR added a 66-line mirror of
`juce::Path::toString` without reading the five call sites of the jam function that
already produced SVG path data. Deleted; `Sheet.cpp` calls `pathToString`.

**Cell width had two sources of truth.** `Manifest.h` held the constant, the manifest held
the row, and the sheet writer read the constant — so editing the table changed the font
but not the cells. The writer now reads the table.

**`Svg::getSVGSize` was dead code.** Declared and defined, called nowhere in jam or ggwp.
Deleted. `jam_SVG.cpp` was also the only header/source pair in `jam_graphics` whose case
did not match its header; renamed to `jam_Svg.cpp`, with the include at
`jam_graphics.cpp:5` updated.

**A manifest with no `## glyphs` table and no sheet on disk produced empty sheets.**
`addMissingGlyphsTable` looked for a sheet to seed from and had no `else`, so the table
stayed missing and `writeWeightSheets` emitted zero cells. The specification covers this
case — table absent and glyphsheet absent is "generate both, table default, glyphsheet
empty". `addGlyphs` now takes a pointer, and a null sheet root yields the default
94-row table.

### Audit

Two sweeps. The first returned roughly sixty findings; the second, over all fifteen sprint
artifacts, returned a further set graded by confidence with a citation each.

Resolved: the background layer order, the resized user rect, id-text matching, the dropped
override, unshifted tags now failing loudly, cell width, the zero-byte sheet that silently
emptied the glyph table, the triple parse-and-validate block, the duplicated error
subexpression, two guide offsets restating metric values, three `getManifestDocument`
entry points collapsed to one, `sectionHeadingLevel` and `sheetCellWidth` scoped to their
users, `addMissingTables` split, four near-identical shift registrations collapsed, the
dead array slot, the unnamed `1`, the `px` suffix, values recomputed per iteration,
single-use constants, `sheetMarginBottom` renamed into its family, `Main.cpp` option
constants, `isGlyphGroup` replaced by the framework filter, the space-glyph temporary,
three bail-out guards in the SVG writers, thirteen copy-initialised locals, and the
pessimistic `rawText` re-check.

`getArtworkSvg` went from 79 lines to 17 by extracting `getShiftedAttributes`.
`getSheetSvg` shed its document assembly into `getDocumentSvg`.

### Doxygen

Five headers verified line by line against their implementations. Two needed no change.
Four blocks corrected: `getCellText`'s rationale, which was factually wrong;
`getManifestDocument`'s missing null case; `getCodepoints`, which did not state the
leading-underscore gate; `getGlyphId`, cross-referenced to its inverse. One unescaped
`\<juce::XmlElement\>` fixed in `jam_XML.h`.

### Reported, Not Resolved

- **No `Doxyfile` in either project.** Only `docs/xml/Doxyfile.xml`, doxygen's own dump
  from a prior run. The zero-warning policy cannot be verified by running doxygen.
- **`jam::Xml` exposes no indent accessor**, so `Sheet.cpp`'s `getIndent` duplicates the
  rule at `jam_XML.h`.
- **`Sheet.cpp` is 393 lines.** A file-level split needs a new file.
- **`getGlyphGroups` decodes each id and discards the result**, and both callers decode it
  again. Removing the second decode needs a public signature change with two external
  callers.
- **`Svg` and `AttributedGraphics` are mutually dependent.** `jam_AttributedGraphics.h`
  defines `struct Svg::Flex`, so it must follow `jam_Svg.h`; `jam_Svg.h` names
  `AttributedGraphics`, defined in that later file. No include order resolves the cycle,
  which is what the forward declarations at `jam_Svg.h:13` and `:16` express.
- **`jam_Svg.h` has no `#pragma once`**, while `jam_XML.h:1` has one. Both are submodule
  headers included once by their module's topmost header.
- **`Svg::Format::stroke` can emit an SVG arc command.** `juce::Path::toString` writes a
  leading `'a'` for even-odd winding, `pathToString` uppercases it to `A`.
  `jam_MermaidGraphics.cpp:361` sets even-odd on a path that reaches that writer.
- **A manifest `type` token outside the known set aborts.** `jam_ConfigDocument.cpp:80`
  passes it into a lookup that throws `std::out_of_range`, uncaught. Reproduced at exit 134.

### Alignment Check

- **B** — Every document is stack-owned in `runInitCommand` and dies with it.
- **L** — `Init.cpp` split into three units with real responsibility boundaries.
  `Sheet.cpp` and `getSheetSvg` still cross; both crossings are recorded above.
- **E (Explicit)** — Failures go through `ConsoleApplication::fail` naming the artifact.
  An artwork tag with no shift rule now fails instead of emitting unshifted markup.
- **S (SSOT)** — The `## glyphs` table is the one inventory, and the sheet writer now reads
  `cellWidth` from the table.
- **S (Stateless)** — The reconcile is a pure function of the two parsed documents.
- **E (Encapsulation)** — The sheet is emitted in one pass. The parsed sheet is read-only;
  nothing mutates a complete document.
- **D** — Idempotence is the test, and it holds across three consecutive runs.

### Debts Paid

None entering this sprint.

### Debts Deferred

None. Every item under *Reported, Not Resolved* awaits ARCHITECT's disposition.

---

## Sprint 3: Model Owns Content, Writer Encodes Only, Decomposition ✅

**Date:** 2026-09-16

### Agents Participated
- **COUNSELOR:** fable-5 — Directed the sprint, validated every landing by direct read
- **PATHFINDER** (invoked by COUNSELOR) — Build runs, byte comparisons, regeneration diffs
- **ENGINEER** (invoked by COUNSELOR) — All implementation, eleven delegations
- **AUDITOR** (invoked by COUNSELOR) — One sweep after the final step, 80 findings

### Result

`ggwp` still writes fonts byte-identical to the reference pipeline. Seven independent
verification rounds, one after every landing:

```
Book   34128 bytes  IDENTICAL
Medium 34160 bytes  IDENTICAL
Bold   32596 bytes  IDENTICAL
```

Each round rebuilt clean with zero warnings from `jam_opentype/` or `ggwp/Source/`.

### Files Modified

- `jam/cast/opentype.md` — `StandardGlyphName`, `RibbiStyle`, `macRomanBytes` (128 rows);
  `kind` → `type` in the `GsubLookupType` brief; `CmapEncodingId` sentence rewrapped
- `jam/cast/lookuptables.md` — wrong-direction `macRomanCodepoints` section removed
- `jam/cast/identifiers.md` — `glyphName`, `nameId`, `ligature`
- `jam/cast/spell.md` — `@code:hash-map` block for `macRomanBytes`; `jam_OpenType.h` file brief
- `jam/jam_opentype/jam_opentype.h` — guarded HarfBuzz include, matching `jam_markdown.h:24-26`
- `jam/jam_opentype/jam_opentype.cpp` — 3 implementation includes to 13
- `jam/jam_opentype/document/jam_OpenTypeDocument.h` / `.cpp` — 667 lines split to 5 files
- `jam/jam_opentype/document/jam_OpenTypeDocumentGlyph.cpp` — new, 219 lines
- `jam/jam_opentype/document/jam_OpenTypeDocumentGsub.cpp` — new, 83 lines
- `jam/jam_opentype/document/jam_OpenTypeDocumentName.cpp` — new, 92 lines
- `jam/jam_opentype/document/jam_OpenTypeDocumentTables.cpp` — new, 131 lines
- `jam/jam_opentype/document/jam_OpenTypeWriter.h` / `.cpp` — 1579 lines split to 7 files
- `jam/jam_opentype/document/jam_OpenTypeWriterMetrics.cpp` — new
- `jam/jam_opentype/document/jam_OpenTypeWriterCmap.cpp` — new
- `jam/jam_opentype/document/jam_OpenTypeWriterName.cpp` — new
- `jam/jam_opentype/document/jam_OpenTypeWriterGlyf.cpp` — new
- `jam/jam_opentype/document/jam_OpenTypeWriterGsub.cpp` — new
- `jam/jam_opentype/document/jam_OpenTypeWriterOs2.cpp` — new
- `jam/jam_opentype/curve/jam_CubicToQuadratic.h` / `.cpp` — tolerance test extracted, throws
- `ggwp/Source/Main.cpp` — option constants, stored glyph index, `const` writer
- `ggwp/Source/Init.cpp` — generated `Id::` keys and types, explicit table rows
- `ggwp/Source/Init.h` — `initOption`
- `ggwp/Source/Help.h` — `@file`

### Problems Solved

- **Three constant tables left the writer for CAST.** `map::macRomanBytes` is keyed by
  Unicode codepoint and valued by Mac Roman byte — the direction its only consumer asks
  for. The previous table was the inverse and was scanned linearly. Every one of the 128
  pairs was cross-checked against HarfBuzz's independent `unicode_to_macroman` before the
  table was authored. `map::StandardGlyphName` and `map::RibbiStyle` replaced two more
  hardcoded writer tables.
- **The Model owns the font's content.** The `name` table is 17 child Elements built at
  creation, each carrying `Id::nameId` and `Id::text`. Each glyph carries `Id::index` and
  `Id::glyphName`. The `cmap` and `gsub` Elements are filled at creation instead of left
  empty. The writer reads and encodes; it decides nothing.
- **Each `glyf` record is built once.** `getGlyfBytes` and `getGlyphRecordSizes` each
  serialised every glyph, and both ran from the dispatch. `getBytes` now builds the records
  once into a creation buffer and the dispatch signature carries them. `getGlyphRecordSizes`
  is gone.
- **Re-derivation removed.** `getMacintoshNameTexts` was rebuilt three times per font and
  `getWindowsNameTexts` twice; both are now built once in `getNameBytes` and passed down.
  `getFirstGlyphs` went from three rebuilds to one. `getGlyphIndexForCodepoint` and
  `validateFont` counted a glyph index that every glyph already stores.
- **Decomposition.** `jam_opentype` went from 3 implementation files to 13, largest 258
  lines, unity-built through `jam_opentype.cpp` on the `jam_markdown.cpp:3-9` precedent.
- **Undefined behaviour in Release.** `addMacRomanText` asserted an iterator was not `end()`
  and then dereferenced it; the assert compiles away in Release. It now reads
  `map::macRomanBytes.at()`, which throws in both builds. `addSheetGlyph` had the same shape
  on a null `<rect>` and now throws `std::out_of_range`.
- **A silently dropped curve.** `getQuadraticSpline` returned an empty array when no spline
  fitted, and `addQuadraticCurve` then computed a negative span count and dropped the cubic
  segment with no signal. It now throws `std::invalid_argument`.
- **Vocabulary borrowed across domains.** The `name` table wrote its platform and encoding
  fields from `map::CmapPlatformId` and `map::CmapEncodingId`. It now declares its own
  constants beside the Macintosh ones already there. The GSUB ligature record was tagged
  `Id::glyf` and is now `Id::ligature`.
- **ggwp manifest keys.** Every key and type literal in the three manifest tables is now a
  generated identifier, including `Id::integer`, `Id::string` and `Id::floatingPoint`. Three
  `jam::Strings` indexed in lockstep became explicit per-row calls.
- **A stale generated header.** Regenerating `jam_Identifiers.h` emitted `axis` and `curve`
  alongside `ligature`. Both rows already existed in `identifiers.md:122` and `:318`, and
  `jam_mermaid_diagram` already consumes them. The generated file had simply never been
  regenerated; ggwp does not build that module, so the drift was invisible.

### Sprint 2 Log Correction

Sprint 2 recorded "`loca` re-serialising `glyf`" as resolved. The code disagreed:
`getGlyfBytes` and `getGlyphRecordSizes` each built every glyph record, and both ran from
the dispatch. This sprint fixed the code. The Sprint 2 entry is left as written and
corrected here.

### Audit

80 findings. Twelve rejected on evidence, the rest resolved:

- **#53 rejected.** `juce::roundToInt` is not equivalent to `getMetricUnit`.
  `juce_MathsFunctions.h:592-594` states the routine "gets its speed at the expense of some
  accuracy, and when rounding values whose floating point component is exactly 0.5, odd
  numbers and even numbers will be rounded up or down differently." `getMetricUnit` is
  deterministic round-half-to-even. Substituting it would violate MANIFESTO **D**.
- **#71 rejected.** The claim that jam siblings use an underscored END OF NAMESPACE banner is
  false. `jam_Buffer.h:523` and `jam_MarkdownDocument.h:1700` use spaces, as jam_opentype does.
- **#31, #32, #33 rejected.** Each proposes unifying a near-duplicate pair whose only
  difference is the accumulated value. Unification needs a selector parameter or a function
  pointer; CODING.md bans unnecessary helpers and manual boolean flags.
- **#61, #62 rejected.** NAMES.md Rule 6 bans neither `validate` nor `write`, and Rule 1:66-70
  lists `void buildInterface();` as correct — `build` is absent from the verb table too. The
  table governs accessor and mutator prefixes, not every function.
- **#10, #11, #13, #15, #16, #18 rejected.** Single-responsibility field writers. MANIFESTO
  L:62-65 — a helper carved out only to move lines below the limit is relocation, not
  decomposition.

Resolved: the Mac Roman and `<rect>` Release-path defects, the dropped cubic, 24
`get<T>(key, default)` calls where the Model guarantees the key, five `juce::ignoreUnused`
shims replaced by unnamed parameters, the `'?'` masking fallback, the fabricated spline,
a `std::pair` fake carrier in `addSheetGlyphs`, three duplicated constants hoisted to class
scope, `hasContours` replacing five copies of one predicate, `getStyleName` replacing four,
`getSegmentEndIndex` replacing two, `getMetricOrZero` replacing three sentinel collapses,
the bare `2` multipliers and the bare pad literal, two redundant `bounds` computations, a
doubled `delta != 0` test, thirteen hoisted ggwp constants returned to their single user,
`getCmapSubtableBytes` 48 → 34 lines, `getCmapBytes` 43 → 37, `getQuadraticSpline` 54 → 32,
`getBytes` 38 → 30, and the `walkedGlyphCount` local whose name stated something false.

### Doxygen

Header-only, six files, 24 blocks added and three corrected. Every `@param` checked name by
name against its signature. `seed` removed from a `@param` line per NAMES.md Rule 6. Stale
`boldStyle` prose corrected to `map::RibbiStyle::bold`. `getToken` gained the two `@param`
lines it lacked. No `@file` was added to a jam submodule header — no jam submodule header
carries one, and `jam_opentype.h` holds the tag for the module.

### Reported, Not Resolved

- **`jam_OpenType.h` and `jam_Bimaps.h` carry no `@file` block.** Nine sibling generated
  headers do, at line 18 each. `spell.md:149-152` and `:154-159` declare a brief for both,
  and the brief never reaches the output. The `spell.md` text was corrected this sprint and
  is inert until the emitter is fixed. The fix belongs to the CAST generator, which is
  outside this sprint's file set.
- **Doxygen warning count unverified.** Neither jam nor ggwp has a Doxyfile or a `doxygen`
  build target. Regeneration is `<leader>bd` in nvim, which no agent can run.

### Alignment Check
- [x] BLESSED principles followed
- [x] NAMES.md adhered
- [x] CODING.md CRITICAL RULES applied
- [x] No bail-out guards
- [x] Fail-fast error handling

### Debts Paid
- None

### Debts Deferred
- None

**Status:** ✅ Byte-identical to reference, clean build, zero in-scope warnings

---

## Sprint 2: Byte-Identical Font Output, Audit Sweep, Doxygen ✅

**Date:** 2026-09-16

### Agents Participated
- **COUNSELOR:** fable-5 — Directed the sprint, validated every step by direct read
- **PATHFINDER** (invoked by COUNSELOR) — Font table dumps, fontTools evidence, compare runs
- **LIBRARIAN** (invoked by COUNSELOR) — fontTools rounding, `post`/`name`/`cmap`/`OS/2` spec facts
- **ENGINEER** (invoked by COUNSELOR) — All implementation
- **AUDITOR** (invoked by COUNSELOR) — One sweep after the final step, 57 findings

### Result

`ggwp` now writes fonts byte-identical to the Python/fontTools pipeline. All three
weights, whole file:

```
Book wholefile: IDENTICAL
Medium wholefile: IDENTICAL
Bold wholefile: IDENTICAL
```

### Files Modified

- `jam/jam_opentype/document/jam_OpenTypeDocument.h` — Model contract; `pointFieldCount`,
  `spaceCodepoint`, `panoseFieldCount`, `getFontUnit`, `getMetricUnit` made public
- `jam/jam_opentype/document/jam_OpenTypeDocument.cpp` — Glyph contours built once at creation
- `jam/jam_opentype/document/jam_OpenTypeWriter.h` — 116 → 589 lines with doxygen
- `jam/jam_opentype/document/jam_OpenTypeWriter.cpp` — cmap, post, name, OS/2, table order
- `jam/jam_opentype/curve/jam_CubicToQuadratic.h` / `.cpp` — One cubic-segment pass
- `jam/cast/identifiers.md` — `contourEnds`, `flags`, `glyphs`, `leftSideBearing`, `regular`
- `jam/cast/opentype.md` — `CmapEncodingId.unicodeBmp`; `SfntTableTag` renumbered to file order
- `jam/cast/lookuptables.md` — `os2UnicodeRangeStarts` / `Ends` / `Bits`, 169 rows each
- `jam/cast/spell.md` — `@os2UnicodeRange` alias and three output blocks
- `ggwp/Source/Main.cpp` — FreeType validation lane, `failIfOptionIsMissing`
- `ggwp/Source/Init.cpp` — `--init`, per-section table ids
- `ggwp/Source/Init.h` — doxygen

### Problems Solved

- **Glyph Model restored to the plan.** The glyph Element stored an SVG path string and the
  writer re-parsed it about twelve times per glyph. It now stores `points`, `flags`,
  `contourEnds` and `leftSideBearing`, built once at creation. `parseSVGPath` and
  `CubicToQuadratic` run once per glyph.
- **cmap.** Added the Unicode-platform encoding record (0, 3) beside (3, 1), sharing one
  format 4 subtable. Format 12 is emitted only when a codepoint exceeds U+FFFF.
- **post.** Format 3.0 (32 B) replaced by format 2.0 (1018 B) with glyph names.
- **name.** Seventeen Macintosh records added, Mac Roman encoded, before the Windows block,
  with identical strings sharing one storage offset.
- **OS/2.** `ulUnicodeRange1/2` computed from the mapped codepoints through a 169-row CAST
  table, verified row-for-row against fontTools' `OS2_UNICODE_RANGES`.
- **Two rounding layers.** `getFontUnit` is fontTools' `otRound`, half up. `getMetricUnit` is
  Python's `round`, half to even, for the manifest metric arithmetic. Both were needed.
- **Table order.** HarfBuzz sorts by blob size then tag. `SfntTableTag` now declares the
  OpenType-recommended order and drives `hb_face_builder_sort_tables`.
- **The escaped-pipe crash.** A manifest cell holding a literal `|` merged two tables. Fixed
  by backtick-wrapping cells through `jam::Format`, without touching jam_markdown.

### Audit

57 findings. Three rejected on evidence, the rest resolved:

- **#2 rejected.** The `.notdef` contour reversal is deliberate compensation — the path is
  authored in the opposite winding to Python's. Byte-identical `glyf` proves it.
- **#55 rejected.** `cu2qu.py:58-59` does carry the `1e-15` epsilon and `:306-307` does carry
  the `b == c` branch. The implementation matches fontTools.
- **#10 rejected.** NAMES Rule 1 sanctions `add` taking its container first and `apply`
  pushing onto a caller-supplied target.

Resolved: dead `Id::path` validation, three hand-rolled required-option guards, four
duplicated metrics lookups, `getCellRect` duplicating `getChildByID`, a built-then-mutated
`juce::Path`, `loca` re-serialising `glyf`, 676 `getMappedCodepoints` rebuilds reduced to 4,
GSUB grouping hoisted out of its inner loops, six `writeRepeatedByte` conversions,
`reservedPad` split into nine named fields, `getNameStrings` 86 → 29 lines, `addGlyphFlags`
53 → 34, `getLigatureGlyphs` 40 → 25, plus guards, duplicate constants and redundant includes.

### Doxygen

Header-only, five files, zero build warnings. Every `@param` checked name by name against its
signature; 82/82 and 30/30 declaration-to-definition reconciliation.

### Alignment Check
- [x] BLESSED principles followed
- [x] NAMES.md adhered
- [x] CODING.md CRITICAL RULES applied
- [x] No bail-out guards
- [x] Fail-fast error handling

### Debts Paid
- None

### Debts Deferred
- None

**Status:** ✅ Byte-identical to reference, clean build, zero doxygen warnings

### Residuals

Three audit findings are open and carry into the next sprint:
- **#11** `OpenTypeWriter` is a god object at 1530 lines. The Mac Roman codec, the UTF-16
  codec and glyph naming are independently meaningful and belong outside it.
- **#15** The `_XXXX` glyph-id wire format has three implementations and no owner.
- **#27** The Mac Roman table, `standardGlyphNames` and `ribbiStyles` are hardcoded in the
  writer where the OS/2 ranges correctly went to CAST. Needs a `cast` run.

---

<!-- Example sprint entry (delete this after first real sprint) -->

## Sprint 1: Project Setup and Initial Planning ✅

**Date:** 2026-01-11  
**Duration:** 14:00 - 16:30 (2.5 hours)

### Agents Participated
- **COUNSELOR:** Kimi-K2 — Wrote SPEC.md and ARCHITECTURE.md
- **ENGINEER** (invoked by COUNSELOR) — Created project structure
- **AUDITOR** (invoked by COUNSELOR) — Verified spec compliance

### Files Modified (8 total)
- `SPEC.md:1-200` — Complete feature specification with all flows
- `ARCHITECTURE.md:1-150` — Initial architecture patterns documented
- `src/core/module.cpp:10-45` — Core module scaffolding with proper initialization
- `src/core/module.h:1-30` — Core module header with explicit dependencies
- `tests/core_test.cpp:1-50` — Test scaffolding following Testable principle
- `CMakeLists.txt:1-25` — Build configuration with explicit targets
- `README.md:1-20` — Project overview

### Alignment Check
- [x] BLESSED principles followed (Bound, Lean, Explicit, SSOT, Stateless, Encapsulation, Deterministic)
- [x] NAMES.md adhered (semantic names, verb-noun functions, no type encoding)
- [x] MANIFESTO.md principles applied (no layer violations, explicit dependencies)
- [x] No early returns used
- [x] Fail-fast error handling implemented

### Problems Solved
- Established project foundation following domain-specific patterns
- Defined clear module boundaries preventing layer violations

### Debts Paid
- `DEBT-20260111T101530` — Resolved missing module.h include in core_test.cpp (see Files Modified)
- *(or)* "None" if sprint did not touch any DEBT.md entries

### Debts Deferred
- `DEBT-20260111T143022` — Performance benchmarking suite (deferred by ARCHITECT command — out of scope for setup sprint)
- *(or)* "None" if no items pushed to DEBT.md during this sprint

**Status:** ✅ APPROVED - All files compile, tests scaffold in place

---

<!-- Actual sprint entries go here, written by PRIMARY agents -->

---

**End of SPRINT-LOG.md Template**

Copy this template to your project root as `SPRINT-LOG.md` and customize:
- Project name
- Repository URL/path
- Starting date
- Add project-specific rules to CRITICAL RULES section

Rock 'n Roll!  
**JRENG!**
