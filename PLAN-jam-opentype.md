# PLAN: jam_opentype on jam::Document

**RFC:** none — objective from ARCHITECT prompt (this session)
**Date:** 2026-09-15
**BLESSED Compliance:** verified against this session's read inventory (citations inline)
**Language Constraints:** C++17 / JUCE / JAM — LANGUAGE.md C++/JUCE Lean adaptation; 30-line function cap applies unchanged (LANGUAGE.md:35)

## Context

The first jam_opentype attempt (Steps 3-4 of the prior plan) produced six hand-rolled
table classes under `jam_opentype/tables/` — fake carriers with serializers attached,
unprefixed filenames, no namespace banners, a new pattern where the framework already
has one. ARCHITECT rejected it. The framework's model is `jam::Document`
(jam_core/document/jam_Document.h): an `Owner<Element>` arena tree, keyed properties,
per-domain subtypes (MarkdownDocument, ConfigDocument, AnsiDocument, CodeDocument), a
Writer family, and Function::Map dispatch. This plan deletes the rejected code and
rebuilds jam_opentype as a Document domain: the font is an Element tree, and one
writer turns it into OpenType bytes.

The output format is OpenType with TrueType outlines (`.ttf`): sfnt container,
`glyf`/`loca` quadratic outlines, `GSUB` ligatures. ARCHITECT ratified the names
`OpenTypeDocument` and `OpenTypeWriter` this session.

## Decisions Already Ratified This Session

- Names: `OpenTypeDocument`, `OpenTypeWriter` (ARCHITECT, this session).
- Curve tolerance: `max_err = 1.0` → named constant `maximumError` (ARCHITECT, earlier this session).
- GSUB: full scope now, not deferred (ARCHITECT, earlier this session).
- CLI argument parsing: Pathfinder rechecks `jam_subprocess` before Step 10 implementation (ARCHITECT, earlier this session).
- Namespace separators are mandatory in every file, opening bar and END OF NAMESPACE banner (ARCHITECT, this session).

## Dependency & API Inventory (all read this session)

| Capability | API | Citation |
|---|---|---|
| Model | `jam::Document` — `Element` (id + intrusive links + `Array<Property<Value>>`), closed `Value` variant, `addChild (Element&, Identifier)`, `Element::add<T> (key, args…)` | jam_Document.h:190, :164-178, :749, :364 |
| Subtype shape | `struct MarkdownDocument : Document`, three protected overrides, private transient accumulator | jam_MarkdownDocument.h:15, :333-350, :372 |
| Writer shape | `MarkdownWriter`: public entry + private static per-construct renderers + `Function::Map` dispatch built once via IIFE static local | jam_MarkdownWriter.h:18, :524, :537; jam_MarkdownWriter.cpp:564-592, :625-631 |
| Dispatch | `jam::Function::Map<Key, Return>` — `add<Args…>` must mirror call-site deduction; `get` verifies via dynamic_cast + assert | jam_Function.h:212-344, :39-61 |
| Big-endian writes | `juce::MemoryOutputStream::writeShortBigEndian / writeIntBigEndian / writeInt64BigEndian`; buffer out via `getData()/getDataSize()` | juce_OutputStream.h:123-147; juce_MemoryOutputStream.h:85-92 |
| Byte container | `juce::MemoryBlock` — jam's writer-side byte idiom | jam_BinaryCodec.h:5-6; jam_TerminalMouse.h doc block |
| sfnt assembly | `hb_face_builder_create / hb_face_builder_add_table / hb_face_reference_blob`, `hb_blob_create / hb_blob_get_data` | hb-face.h:208-218, :123; hb-blob.h:86-91, :153 |
| SVG outline read | `jam::Svg::getPath (String/Element, ElementType)`, `getElementPath`, `getElementId`, `getSVGSize` | jam_Svg.h:157, :278-335, :391, :964 |
| Path walk/build | `juce::Path::Iterator` (elementType + x1..y3), `quadraticTo`, `cubicTo`, `getBounds` | juce_Path.h:732-767, :271-315, :106 |
| Config read | `jam::ConfigDocument::parse`, `getValueTypes()`, `getValueTree` | jam_ConfigDocument.h:19-59; jam_ConfigDocument.cpp:5-82 |
| Manifest write | `jam::MarkdownWriter : Document::Writer` | jam_MarkdownWriter.h:18 |
| Round-trip validation | raw FreeType via jam_freetype (vendor wrapper, zero API): `FT_New_Memory_Face`, `FT_Load_Glyph`, glyph metrics | jam_freetype.h:1-26; jam_GlyphAtlasFreeType.cpp:54-198 |
| Vocabulary authoring | `jam/cast/identifiers.md` flat table `type|name|value|comment`, `@id` rows, value column carries the wire string | identifiers.md:20-150; spell.md:208-213; code.cast:45-46 |
| Numeric vocabularies | Step-2 bimaps survive: CmapPlatformId, CmapEncodingId, NameId, Os2FsSelection, PanoseField, GsubLookupType, SfntTableTag | jam/cast/opentype.md; generated/jam_OpenType.h |

**Explicitly rejected re-implementations:** no big-endian primitives added to
`jam_core/binary_codec` (JUCE supplies them — CODING.md "do not write again a behavior
that the framework supplies"; BinaryCodec is little-endian by contract,
jam_BinaryCodec.h:3, and has zero call sites today). No sfnt directory/checksum code
(HarfBuzz supplies it). No new container, no new dispatch mechanism.

## Design

**Model.** `jam::OpenTypeDocument : Document` in `jam_opentype/document/`. The font
is one Document: root children are table elements keyed by `Id::` table tags
(`Id::head`, `Id::hhea`, `Id::maxp`, `Id::hmtx`, `Id::post`, `Id::cmap`, `Id::name`,
`Id::os2`, `Id::glyf`, `Id::loca`, `Id::gsub`); each table's fields are properties on
its element; glyphs are children of the `glyf` element (one Element per glyph:
codepoint, advance, left side bearing, contours as `Numbers` arrays). Every stored
value maps onto an existing `Value` alternative (int, uint32, String, Numbers,
Elements — jam_Document.h:164-178). The tree is built at creation, complete at
creation (MANIFESTO E), from two existing engines: `ConfigDocument` (metrics, names,
weights, ligature tables) and `jam::Svg` (glyph outlines). Construction happens in the
constructor — creation may use multiple passes and work buffers (MANIFESTO E,
"Creation is not mutation").

**Abstract-base satisfaction.** `Document`'s three pure virtuals are parse-side and
only invoked via `parse()` (jam_Document.h:871-901). OpenTypeDocument is constructed,
never parsed. The three overrides exist as out-of-contract stubs (`jassertfalse`
bodies; precedent for out-of-contract language: jam_Document.h:379-383). This is the
one place the design carries dead-looking surface; it is the cost of the abstract
base, flagged in Risks.

**Writer.** `jam::OpenTypeWriter` in `jam_opentype/document/`. Mirrors
MarkdownWriter's shape exactly, with bytes in place of text (it does not inherit
`Document::Writer` — that contract returns `juce::String`, jam_Document.h:623):

- public: `juce::MemoryBlock getBytes (const OpenTypeDocument&) const` and
  `bool toFile (const OpenTypeDocument&, const juce::File&) const` (machinery shape
  mirrors jam_Document.h:626).
- private: one `static juce::MemoryBlock get<Table>Bytes (const Element&)` per table;
  dispatch via `static const jam::Function::Map<juce::String, juce::MemoryBlock>&
  getTables()` keyed by the table element's own id, built once via the IIFE static
  local idiom (jam_MarkdownWriter.cpp:564-592).
- every emitter writes through a local `juce::MemoryOutputStream` using the
  `write*BigEndian` family; the finished stream hands its bytes to a MemoryBlock.
- `getBytes` walks root children, feeds each table's MemoryBlock to
  `hb_face_builder_add_table` (tag from the element id via the SfntTableTag bimap /
  `hb_tag_from_string`), then `hb_face_reference_blob` returns the whole font. The
  writer IS the assembler — no separate FontAssembler unit.
- all magic numbers named `static constexpr` with unit-stating briefs later
  (MarkdownWriter precedent :45-83); numeric IDs come from the Step-2 bimaps.

**Curve reduction.** `jam::CubicToQuadratic` in `jam_opentype/curve/` —
`static juce::Path getQuadraticPath (const juce::Path& source)`. Walks
`Path::Iterator`, passes through lines/quadratics, subdivides cubics per
cu2quPen.py's error-bounded behavior (clean-room), `static constexpr float
maximumError { 1.0f }`. Consumed by the glyf emitter.

**File conventions (every new file):** `jam_PascalCase.h` + `.cpp` split
(declarations in headers, bodies in .cpp — CODING.md placement table), zero includes
in submodule files (umbrella includes everything), namespace opening bar +
END OF NAMESPACE closing banner, `struct` (model/writer family precedent), no
comments/doxygen until the post-audit pass (banners are structure, not documentation).

## Validation Gate

Each step is validated by COUNSELOR before proceeding — against MANIFESTO.md
(BLESSED), NAMES.md, ~/.carol/CODING.md, LANGUAGE.md, and this plan. Validation is a
direct Read of the produced files, never the subagent's report alone. @Auditor runs
ONCE, after the final step.

## Steps

### Step 1: Delete the rejected code
**Scope:** `jam_opentype/tables/` (Head.h, Hhea.h, Maxp.h, Hmtx.h, Post.h, Cmap.h), `jam_opentype.h` include list.
**Action:** Engineer deletes `tables/` entirely; umbrella keeps module declaration + framework includes only. Nothing else touched.
**Validation:** ggwp toolchain builds clean with the emptied module.

### Step 2: Vocabulary
**Scope:** `jam/cast/identifiers.md` (new `@id` rows, alphabetical), regeneration.
**Action:** Engineer adds the table-tag and field-key identifiers the schema needs — tags: `cmap`, `glyf`, `gsub` (value `GSUB`), `hhea`, `hmtx`, `loca`, `maxp`, `os2` (value `OS/2`), `post` (existing: `head`, `name`); field keys per table schema (e.g. `unitsPerEm`, `advance`, `leftSideBearing`, `codepoint`, `contours`, plus the exact set the Step 3 schema enumerates). Wire-string transforms live in the value column (precedent: identifiers.md:77, :88). Run `cast spell.md`; Step-2 bimaps in `opentype.md` stay as they are.
**Validation:** generated `jam_Identifiers.h` carries every new row; no other generated file drifts.

### Step 3: OpenTypeDocument
**Scope:** `jam_opentype/document/jam_OpenTypeDocument.h` + `.cpp`, umbrella include.
**Action:** Engineer implements `struct OpenTypeDocument : Document`: constructor builds the complete tree from `(const ConfigDocument&, const MarkdownDocument&-parsed glyphsheet inputs via jam::Svg)` using `addChild`/`Element::add<T>` only; three out-of-contract stubs; domain query API (`getTable (const juce::Identifier&)`, glyph iteration) mirroring MarkdownDocument's table query family (jam_MarkdownDocument.h:59-115).
**Validation:** compiles; every property type is an existing `Value` alternative; no fake-carrier structs; 300/30/3; conventions.

### Step 4: CubicToQuadratic
**Scope:** `jam_opentype/curve/jam_CubicToQuadratic.h` + `.cpp`, umbrella include.
**Action:** Engineer implements the error-bounded cubic→quadratic subdivision over `Path::Iterator`, clean-room from cu2quPen.py behavior; `maximumError { 1.0f }`.
**Validation:** compiles; behavior traced against cu2quPen.py's algorithm point by point; quadratic-only output (no cubicTo in result — checked by iterating the result).

### Step 5: OpenTypeWriter — fixed-layout tables
**Scope:** `jam_opentype/document/jam_OpenTypeWriter.h` + `.cpp`, umbrella include.
**Action:** Engineer implements the writer skeleton (public getBytes/toFile, getTables() dispatch, hb assembly) plus emitters for `head`, `hhea`, `maxp`, `hmtx`, `post`.
**Validation:** field order matches the OpenType spec per table; zero magic numbers; dispatch idiom verbatim per jam_MarkdownWriter.cpp:564-592; functions ≤30 lines.

### Step 6: cmap, name, OS/2 emitters
**Scope:** same files.
**Action:** format-4 cmap (segments from contiguous codepoint runs, idDelta/idRangeOffset cases, sentinel); name records with Windows family/subfamily split for non-RIBBI weights; OS/2 from bimaps (fsSelection, PANOSE, weight).
**Validation:** segment boundaries correct for one contiguous and one non-contiguous codepoint set; every platform/encoding/nameID/bit constant traces to a bimap.

### Step 7: glyf and loca emitters
**Scope:** same files.
**Action:** glyf contour flags + delta coordinates from CubicToQuadratic output; loca offsets (indexToLocFormat consistent with head).
**Validation:** encoding traced against _g_l_y_f.py compile path; loca offsets consistent with emitted glyf lengths.

### Step 8: GSUB emitter
**Scope:** same files.
**Action:** ScriptList/FeatureList/LookupList + LigatureSubst format 1, longest-sequence-first ordering, full scope.
**Validation:** a 2-glyph and a 3-glyph ligature sharing a first glyph resolve longer-first, against the config's LIGATURES tables.

### Step 9: Round-trip validation lane
**Scope:** ggwp `Source/` (validation entry, shape decided at Step 10 CLI), using jam_freetype raw FT_* calls.
**Action:** load the written font via `FT_New_Memory_Face`; check glyph count, cmap lookups, advance widths against the source config.
**Validation:** all three checks pass on the reference sheet.

### Step 10: ggwp CLI
**Scope:** `ggwp/Source/` (replaces placeholder Main.cpp).
**Action:** Pathfinder first — recheck `jam_subprocess`/`jam_core` for an existing argument-parsing convention; report before implementation. Then `ggwp --init <font-name>` (blank sheet via `Svg::Format` + manifest via `MarkdownWriter`) and `ggwp --build <sheet>.svg <name>.ttf` (ConfigDocument + Svg → OpenTypeDocument → OpenTypeWriter::toFile).
**Validation:** both commands run end-to-end on the reference sheet; output opens in FreeType; glyph count/cmap/name fields match the Python pipeline's output for the same input.

## BLESSED Alignment

- **B:** Document owns every Element via `Owner<Element>` (jam_Document.h:903); writer owns nothing persistent; MemoryBlocks are scope-local at creation.
- **L:** one writer + Function::Map replaces eleven table classes; 300/30/3 enforced per step; no FontAssembler unit (YAGNI — the writer assembles).
- **E:** every constant named; all inputs visible in signatures; no bail-out guards; stubs assert loudly.
- **S (SSOT):** vocabulary in CAST (identifiers + bimaps) only; the Document is the one model; no field stored twice.
- **S (Stateless):** emitters are static pure functions Element→bytes; the writer holds no state.
- **E (Encapsulation):** jam_core stays free of font knowledge (precedent: jam_MarkdownDocument.h:11-13); OpenType vocabulary lives in jam_opentype; unidirectional — jam_opentype writes, jam_freetype reads.
- **D:** same config + same SVG → byte-identical `.ttf`; verified by the Step 9/10 round trip.

## Risks / Open Questions

1. **Out-of-contract stubs on OpenTypeDocument** — the three parse hooks never run.
   Decided as `jassertfalse` stubs this sprint; if ARCHITECT prefers a jam_core refit
   (splitting the parse contract out of the abstract base), that is a separate
   ARCHITECT-directed sprint.
2. **`Value` variant fit** — if any glyph field genuinely cannot map onto an existing
   alternative, that is a STOP (extending the variant is a jam_core decision,
   jam_Document.h:164-178 plus the two setSpanOffsets exhaustive matches).
