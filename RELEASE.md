# ggwp v1.0

GlyphSheet Generation Workflow Protocol

## What's New

- Release lane: a signed macOS pkg and Windows x64 and arm64 installers, uploaded to the GitHub release.
- Signing data lives in cast/signing.md, and entitlements.plist is generated from it.
- Readers are const end to end; the ggwp sources carry no const_cast.
- --init is idempotent and table-driven.

## Platforms

| Platform | Architecture       | Format                            |
| -------- | ------------------ | --------------------------------- |
| macOS    | universal          | .pkg (signed, notarized)          |
| Windows  | x64                | .exe installer                    |
| Windows  | arm64              | .exe installer                    |

## Installation

macOS: open the .pkg. It installs `ggwp` on your PATH.

Windows: run the installer. It installs `ggwp.exe` and adds its folder to your user PATH.
