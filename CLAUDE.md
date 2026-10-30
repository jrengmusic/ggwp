# CLAUDE.md — ggwp

## Identity

- **Name:** ggwp — GlyphSheet Generation Workflow Protocol
- **Type:** C++17 console app (`juce_add_console_app`), product name `ggwp`
- **Version:** 1.0 (`project(ggwp VERSION "1.0")`, `CMakeLists.txt:32-36`)
- **Stack:** C++17, JUCE 8.0.14 (exact match required, `CMakeLists.txt:79`), CMake ≥ 3.25
- **Modules:** `jam_core`, `jam_freetype`, `jam_graphics`, `jam_markdown`, `jam_opentype` (`CMakeLists.txt:95-99`)
- **Root:** `/Users/jreng/Documents/Poems/dev/ggwp`
- **Sibling paths:** JUCE at `../../JUCE`; JAM at `../jam`

## Current State

- **Last sprint:** Sprint 7 — Release lane (macOS pkg, Windows NSIS). Date 2026-10-05. Source: `carol/SPRINT-LOG.md`.
- **Open state from Sprint 7:** Debug build fails to compile. Missing symbols: `Id::guides`, `Id::heading`, `Id::regular`, `Id::xmlns`, `jam::Format::escape`. Affected files: `Init.cpp`, `Manifest.cpp`, `Sheet.cpp`, `Guides.cpp`.
- **Active plan:** `PLAN-const-only-readers.md` (root). Delete it after ARCHITECT confirms green builds.
- **Active ODE:** none.
- **Active debt:** none. `DEBT.md` does not exist in this project.

## Layer Order and Source

Include graph, verified from `Source/*.cpp` includes. Lower layers do not include higher layers.

| Layer | Files | Role |
|---|---|---|
| 0 (leaves) | `Manifest.{h,cpp}`, `Artwork.{h,cpp}`, `Help.h` | Manifest document unit; artwork; help text |
| 1 | `Guides.{h,cpp}` | Guide lines; uses Manifest |
| 2 | `Sheet.{h,cpp}` | One-pass reconcile and SVG emitter; uses Manifest, Artwork, Guides |
| 3 | `Init.{h,cpp}` | `--init` four-state dispatch; uses Manifest, Sheet |
| 4 (entry) | `Main.cpp` | Option parsing; uses Help, Init, Manifest |

- `Source/generated/ProjectInfo.h` — generated. Do not edit.
- `Source/HELP.md` — help source text. Bundled through `juce_add_binary_data` (`CMakeLists.txt:162-172`).

## Key Docs

| File | Purpose |
|---|---|
| `README.md` | User guide: the build chain |
| `GLYPHSHEET.md` | Glyph sheet format |
| `project-info.md` | Project manifest. Source for the generated `CMakeLists.txt` |
| `cast/cmake.cast`, `cast/spell.md`, `cast/signing.md`, `cast/installer.cast` | CAST template and spell specs |
| `RELEASE.md` | Generated release notes |
| `PLAN-const-only-readers.md` | Active plan (see Current State) |
| `carol/SPRINT-LOG.md` | Sprint history. Written by primaries on `log sprint` only |
| `SPEC.md`, `ARCHITECTURE.md` | Absent in this project |

## Doxygen

Project and active libraries only.

| Scope | Location |
|---|---|
| ggwp | `docs/xml/index.xml` |
| JAM | `../jam/docs/xml/index.xml` |
| JUCE | `../../JUCE/docs/xml/index.xml` |

- Excluded: other frameworks and projects under `~/Documents/Poems/` (KANJUT, CIUM, cast, whatdbg, and others).
- No `Doxyfile` exists in this project. `docs/xml/Doxyfile.xml` is a dump from an earlier run. Regeneration commands are not recorded here.

## Build

- `CMakeLists.txt` is generated. Its header reads "DO NOT EDIT". Edit `cast/*.cast` and `project-info.md`, then regenerate with the `cast` tool. The regeneration command is not recorded in this repo.
- Configure: `cmake -S . -B <build-dir>` from the project root. CMake fails if JUCE is not 8.0.14.
- Apple builds use universal binaries (`x86_64;arm64`) and deployment target 11.0.
- Release on Apple runs the sign, pkg, notarize, and staple chain (`CAST_SIGN`, default ON). It needs the Developer ID certificates and the `notary` keychain profile. Output goes to `~/Documents/Poems/dev/___builds___/`.
- Every build copies the binary to `~/.local/bin/ggwp` (`CMakeLists.txt:260-265`).
- Agents do not build. Build only when ARCHITECT asks.
- `compile_commands.json` exists at the root for tooling.
