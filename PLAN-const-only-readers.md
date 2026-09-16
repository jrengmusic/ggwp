# PLAN: Const-Only Reader Law (Option B)

**RFC:** none — objective from ARCHITECT prompt ("/goplan for B. this means absolute immutable law. const only.")
**Date:** 2026-09-17
**BLESSED Compliance:** verified
**Language Constraints:** C++17 / JUCE / JAM. No deducing-this (C++23). Single-body deduced-constness via `static` templates. LANGUAGE.md C++ Lean override applies (header-only classes stay header-only).

## Context

ARCHITECT ruled Option B on the mutation fork: the reader API is const-only, an
absolute immutable law, enforced by the compiler. The stopped Engineer left the tree
in the Option A shape: dual const/non-const reader overloads, with 30 `const_cast`
occurrences (16 in jam_MarkdownDocument.cpp const-body delegation, 13 in cast/Source,
1 in jam_Document.h `Element::get<T>` const delegation). `const_cast` is banned
outright. This plan deletes the mutable reader family, closes every shallow-const
leak so a `const` document gives no mutable interior access, and rewrites cast's
creation lane to navigate structurally instead of through mutable readers.

**ARCHITECT boundary ruling (this session):** add/remove child stays valid — the
TextEditor needs free line add/remove at arbitrary index. The law binds READERS only.
The mutation/creation API (`addChild`, `appendChild`, `appendChildren`, Index line
lane) is untouched. A live document is a state machine, held mutable, mutated through
its own API. A complete document is held `const`; through `const` no mutable interior
access compiles.

## The Law (enforcement model)

- Document-level reader family (MarkdownDocument `getTable*`, `getList*`,
  `getCodeBlock`, `getBlockquote`, `getListItem`, `getTableList`,
  `getTableHeaderRow`): **const-only**. Non-const overloads deleted forever.
- Element-level access (`get<T>`, `getChildByID`, range-for iteration): **dual**,
  via single-body deduced-constness templates — zero `const_cast`, zero duplicated
  bodies. Mutable variants are reachable only through a mutable document/element
  chain. Post-creation code holds `const` and cannot reach them.
- `Document::root` goes protected; public dual `getRoot()` closes the shallow leak
  (`constDoc.root->add<T>(...)` compiles today; after this plan it does not).
- `Element::parent` becomes `const Element*` (children never mutate parents; no
  structural write through `->parent` exists — verified by grep over jam and cast).
  This lets const reader bodies build keyed probes (`probe.parent = &table`) without
  any cast.
- Zero `const_cast` in jam_core/document, jam_markdown, cast/Source.

## Why cast is NOT a two-tree copy

The literal "construct tree B by deep copy" variant is refuted by evidence:
Token spans in element properties address the owning Document's `source` bytes;
`appendChildren` appends the source and rebases every span via `setSpanOffsets`
(jam_Document.h:816-820, :870-887). A hand-rolled copy-walk would re-implement
source merging and span rebasing — a "use the framework API fully" violation
(CODING.md CRITICAL RULES) with high defect risk. Instead: adoption (move) stays,
and all enrichment remains inside the creation window (MANIFESTO E: "Creation is
not mutation. Creation can use multiple passes... The contract starts at the end
of creation."), navigating structurally — never through the reader family, never
through `const_cast`.

## Dependency & API Inventory

- jam_core: `Document` (jam_Document.h — root :669, elements Owner :923,
  addChild :769, appendChildren :826, setSpanOffsets :870-887), `Element`
  (Iterator :216-227, get<T> :242-257, getChildByID :233, parent :200,
  applyFunctionRecursively :337), `jam::Owner::find` (jam_Owner.h:245).
- jam_markdown: MarkdownDocument reader family (jam_MarkdownDocument.h:64-425,
  .cpp:39-531), provenance stamping pass (.cpp:39-48), `isTableBorder` (.cpp:95 —
  sibling precedent for the new `isTable` predicate).
- cast/Source: Model.h (creation lane :814-948, reader const_casts :288, :326,
  :440, :726), Validator.h (:233, :594, :603, :609, :1230, :1505), Writer.h
  (:66, :214), Shapes.h (:95).
- Consumers: end, eve, ggwp, whatdbg, jam modules (compiler enumerates; ARCHITECT
  builds).

## Name Inventory (verbatim — Engineer has zero latitude)

- `ElementType`, `DocumentType` — template parameter names, ratified this session.
- `isTable` — new static predicate on MarkdownDocument, joins the `isTableBorder`
  family (NAMES Rule 5, nearest sibling — jam_MarkdownDocument.cpp:95).
- `getRoot` — new dual accessor on Document (`get` family). Member `root` keeps its
  name, goes protected.
- No other new names.

## Validation Gate

COUNSELOR validates each step against MANIFESTO.md, NAMES.md, CODING.md, and this
plan before the next step. @Auditor runs ONCE, after the final step. ARCHITECT
builds; agents never build.

## Steps

### Step 1: jam_core Element deep-const surface
**Scope:** jam/jam_core/document/jam_Document.h (+ .cpp fallout in jam_core only)
**Action:**
- Template `Element::Iterator` on `ElementType` (`ElementType* current`,
  `operator*` returns `ElementType*`). `begin()/end()`: non-const overloads return
  `Iterator<Element>`, const overloads return `Iterator<const Element>`.
- `Element::get<T>`: one `static` template body
  (`template <typename ValueType, typename ElementType>`), two one-line member
  overloads. Delete the `const_cast` delegation at :253-257.
- `Element::getChildByID`: same single-static-body pattern; const overload returns
  `const Element*`.
- `Element::parent` → `const Element*`. Report (do not silently patch) any site
  that writes through `->parent`.
- Update the law doxygen at :190-194: readers are const-only; mutable access
  exists only through a mutable document chain (state updates and creation).
**Validation:** zero `const_cast` in the file; single body per accessor; range-for
over `const Element&` yields `const Element*`.

### Step 2: jam_core Document root closure
**Scope:** jam_Document.h; every `.root` / `->root` consumer across jam and projects
**Action:**
- Move `Element* root` (:669) to `protected`.
- Add public dual accessors: `Element* getRoot() noexcept` and
  `const Element* getRoot() const noexcept` (header inline, one line each).
- Migrate external consumers (`document.root->…`, `*document.root`) to
  `getRoot()`. Subclass build()/creation internals keep direct member access.
**Validation:** through `const Document&` no mutable Element is reachable;
consumers compile against the accessor only.

### Step 3: jam_markdown const-only reader family
**Scope:** jam_markdown/document/jam_MarkdownDocument.{h,cpp}
**Action:**
- Delete every non-const reader overload (h and cpp): getTables ×2, getTableRows ×2,
  getTableRow ×2, getTableCell ×3, getTableList, getCodeBlock, getBlockquote,
  getList, getListItem, getTable, getTableHeaderRow.
- The const overload becomes the single real body: former non-const logic, over
  const iteration and const keyed probes (`probe.parent = &table` — legal after
  Step 1). All 16 `const_cast` delegations die. Return types: `const Element*` /
  `jam::Array<const Element*>`.
- Extract `static bool isTable (const Element&) noexcept` (public, beside
  isTableBorder). getTables' filter and the stamping pass both use it (SSOT).
- Rewrite the provenance stamping pass in `parse (text, origin)` (.cpp:39-48) as a
  structural creation walk over the mutable local document
  (`for (auto* table : *document.root)` + `isTable`), not through the reader family.
- Update the law doxygen at .h:15-18 to const-only wording.
**Validation:** zero `const_cast`; zero duplicated bodies; one reader family,
all const.

### Step 4: cast Model creation-lane rewrite
**Scope:** cast/Source/Model.h
**Action:** inside the creation window (`parse (Model&, …)` and its helpers), replace
every mutable-reader navigation with structural navigation on the mutable document:
- `document.getTables()` → walk `*document.root` with `jam::MarkdownDocument::isTable`.
- `document.getTableRows (*table)` → walk `*table`, skip the header row
  (const `getTableHeaderRow` result compares fine against mutable pointers) and
  `isTableBorder` rows.
- `document.getTableCell (row, colId)` → `row.getChildByID (colId)` (non-const
  Element API from Step 1).
- Read-only params retyped const where the callee only reads (e.g. `addValues`'
  header-row param — `row.parent` is `const Element*` after Step 1).
- The four reader-lambda `const_cast`s (:288, :326, :440, :726): where the found
  element is only read, the accumulator becomes `const Element*`; where the caller
  stamps it (creation), the find moves to the mutable structural chain instead.
**Validation:** zero `const_cast` in Model.h; every mutation reaches its element
through a mutable chain rooted at the under-construction `Model&`; the 25 read-only
const signatures unchanged.

### Step 5: cast Validator / Writer / Shapes const propagation
**Scope:** cast/Source/Validator.h, Writer.h, Shapes.h
**Action:** these read a complete Model — retype to const end-to-end:
- Validator :233, :594, :603, :609, :1230, :1505 — `failingLine` and candidates
  become `const Element*`; mutable-overload calls become const-family calls. If any
  site genuinely mutates the complete Model, STOP and report (contract discrepancy).
- Writer :66, :214 — `toFile` pipeline takes `const Model::Element&` /
  `jam::Array<const Model::Element*>`.
- Shapes :95 — `getFirstLine` accumulator becomes `const Element*`.
**Validation:** zero `const_cast` in cast/Source; post-creation code holds const only.

### Step 6: cross-project call-site sweep
**Scope:** remaining jam modules, end, eve, ggwp, whatdbg — call sites of the deleted
overloads, `root` member access, iteration fallout
**Action:** retype holders to `const Element*` / const document references; migrate
`root` access to `getRoot()` (ggwp Manifest.cpp/Init.cpp creation lanes use the
mutable `getRoot()` on their under-construction ConfigDocument). Compiler output is
the ground truth for the sweep — ARCHITECT builds each project and returns errors.
**Validation:** per-file read against this plan; reads are const; creation lanes use
creation API + mutable chains only.

### Step 7: contract text
**Scope:** ~/.carol/CODING.md ("Readers return const" rule + checklist line);
law doxygen already updated in Steps 1 and 3
**Action:** tighten the rule to: "**Readers return const.** A reader never returns a
mutable pointer or reference into owned state. A document's reader API is const-only.
A complete document is held `const`. Creation and state-update code holds the
document mutably and reaches elements through the creation API and structural
navigation it owns — never by re-finding through readers."
**Validation:** wording matches the implemented law exactly.

## BLESSED Alignment

- **B** — ownership unchanged; adoption stays move-based (no copies, no lifetime
  ambiguity).
- **L** — deletion-heavy; single bodies replace dual bodies; no new files.
- **E (Explicit)** — the const boundary makes the creation window visible in every
  signature; fail-fast unchanged.
- **S (SSOT)** — one body per reader; `isTable` collapses the duplicated table
  filter; no shadow trees (two-tree copy rejected).
- **S (Stateless)** — transient creation buffers only, inside the creation window.
- **E (Encapsulation)** — `root` stops leaking mutable interior access; readers
  cannot mutate.
- **D** — a complete document is compiler-provably immutable to all consumers.

## Verification (end-to-end, after ARCHITECT builds)

1. ARCHITECT builds jam consumers: cast, ggwp, END, eve, whatdbg — clean.
2. cast regen over END/eve data — generated files byte-identical.
3. Pathfinder re-verifies ggwp byte-identity (display-serif refs 34128/34160/32596)
   and --init ×3 idempotence in a scratch dir.
4. `grep -rn const_cast` over jam_core/document, jam_markdown, cast/Source → zero.
5. After the sprint settles: jam → KANJUT `cast --sync` re-run, jfs rebuild
   (ARCHITECT's call).

## Risks / Open Questions

- **jam_Owner.h internal const_casts (:70, :251):** pre-existing, generic container
  internals (lookup-key cast inside `Owner::find`), outside the named scope. Listed
  for ARCHITECT's disposition; untouched by this plan.
- **`Element::parent` constness:** grep found no structural write through
  `->parent`; if the compiler surfaces one, Engineer reports it — no silent fix.
- **END Index / TextEditor lane:** untouched per ARCHITECT's ruling — add/remove
  child and mutable line access remain valid on a document held mutable.
