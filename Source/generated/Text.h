/*******************************************************************************
                        Codegen Annotated Source of Truth
————————————————————————————————————————————————————————————————————————————————

            ░░████████████░░████████████░░████████████░░████████████
            ░░████  ░░████░░████  ░░████░░████  ░░████    ░░████
            ░░████        ░░████  ░░████░░████            ░░████
            ░░████        ░░████████████░░████████████    ░░████
            ░░████        ░░████  ░░████        ░░████    ░░████
            ░░████  ░░████░░████  ░░████░░████  ░░████    ░░████
            ░░████████████░░████  ░░████░░████████████    ░░████

————————————————————————————————————————————————————————————————————————————————
                         FOR YOUR EYES ONLY, DO NOT EDIT
********************************************************************************/

/**
 * @file Text.h
 * @brief ggwp message strings — one constant per failure.
 */

#pragma once

namespace text
{
/*_____________________________________________________________________________*/

/** @brief ggwp message texts, emitted verbatim when a run fails. */
struct Diagnostics
{
    static constexpr const char* const failSheetNotFound        { "glyph sheet not found"             };///< A weight's glyph sheet file does not exist.
    static constexpr const char* const failRoundTrip            { "round trip validation failed"      };///< Prefix of every failure of the load-back check.
    static constexpr const char* const failLoadGlyph            { "FT_Load_Glyph failed"              };///< FreeType could not load a glyph.
    static constexpr const char* const failGlyphAdvance         { "glyph advance differs"             };///< The loaded glyph's advance differs from the model.
    static constexpr const char* const failCharIndex            { "character map entry differs"       };///< A codepoint maps to another glyph than the model's.
    static constexpr const char* const failGlyphOutline         { "glyph has no outline contours"     };///< A glyph that the model draws has no contour in the font.
    static constexpr const char* const failGlyphCount           { "glyph count differs"               };///< The font's glyph count differs from the model's.
    static constexpr const char* const failWeightUnknown        { "unknown weight"                    };///< The weight is not a table of the manifest.
    static constexpr const char* const failFontWrite            { "failed to write font file"         };///< The font file could not be written.
    static constexpr const char* const failManifestWrite        { "failed to write manifest file"     };///< The manifest file could not be written.
    static constexpr const char* const failSheetsDirectory      { "failed to create sheets directory" };///< The sheets directory could not be created.
    static constexpr const char* const failSheetWrite           { "failed to write glyph sheet"       };///< A glyph sheet file could not be written.
    static constexpr const char* const failManifestCell         { "manifest table cell missing"       };///< A manifest table row has no cell for a column.
    static constexpr const char* const failGlyphRowReserved     { "reserved glyph row"                };///< A glyphs row holds the notdef or the space.
    static constexpr const char* const failGlyphRowUnsupported  { "unsupported glyph row"             };///< A glyphs row has an unsupported shape.
    static constexpr const char* const failGlyphGroupRect       { "glyph group has no rect cell"      };///< A glyph group of a sheet has no rect cell.
    static constexpr const char* const failArtworkShift         { "artwork tag has no shift rule"     };///< An artwork element has a tag that the sheet cannot shift.
    static constexpr const char* const failSheetXml             { "glyph sheet is not valid XML"      };///< A glyph sheet does not parse.
    static constexpr const char* const failFontFace             { "font cannot be opened"             };///< The font file given to the extract command cannot be opened.
    static constexpr const char* const failBlankGlyph           { "glyph has no outline"              };///< A glyph above the space has an advance and no outline.
    static constexpr const char* const failUnmappedSubstitution { "substitution output unmapped"      };///< A substitution output has no codepoint and is no ligature.
};
/** @brief ggwp command texts, passed to the console application, and the success line prefix. */
struct Commands
{
    static constexpr const char* const helpShortDescription       { "Prints this guide"                                                                                                                                                                                                                                                                                                                                                                 };///< The short description of the help command.
    static constexpr const char* const helpLongDescription        { "Prints the GGWP guide."                                                                                                                                                                                                                                                                                                                                                            };///< The long description of the help command.
    static constexpr const char* const buildArgumentDescription   { "--build=<manifest.md> --weight=<weightTableId> --out=<output.ttf>"                                                                                                                                                                                                                                                                                                                 };///< The argument description of the build command.
    static constexpr const char* const buildShortDescription      { "Builds a TTF from a manifest and glyph sheet, then validates the round trip"                                                                                                                                                                                                                                                                                                       };///< The short description of the build command.
    static constexpr const char* const buildLongDescription       { "Parses the manifest, resolves the weight table's glyph sheet relative to the manifest's directory, writes the font, and validates it by loading it back through FreeType."                                                                                                                                                                                                         };///< The long description of the build command.
    static constexpr const char* const extractArgumentDescription { "--extract=<font.ttf> --manifest=<manifest.md> --weight=<weightTableId>"                                                                                                                                                                                                                                                                                                            };///< The argument description of the extract command.
    static constexpr const char* const extractShortDescription    { "Extracts a glyph sheet from a TTF, using a manifest's metrics"                                                                                                                                                                                                                                                                                                                     };///< The short description of the extract command.
    static constexpr const char* const extractLongDescription     { "Reads the font's character map and its ligatures, normalises the outlines from the font's units per em to 1024, and draws each glyph into its own cell. The manifest's metrics are read and never written. A manifest without a glyphs table gains one, read from the extracted sheet. The command writes the weight's glyph sheet."                                               };///< The long description of the extract command.
    static constexpr const char* const initArgumentDescription    { "--init=<font-name>"                                                                                                                                                                                                                                                                                                                                                                };///< The argument description of the init command.
    static constexpr const char* const initShortDescription       { "Creates a missing manifest or glyph sheet, or reconciles the two"                                                                                                                                                                                                                                                                                                                  };///< The short description of the init command.
    static constexpr const char* const initLongDescription        { "Creates whatever is missing: a default manifest, or a matching glyph sheet. An existing manifest gains a missing glyphs table, read from its glyph sheet. GGWP then reconciles every weight's glyph sheet against the manifest's glyph table. It adds and removes cells, and re-lays out the canvas. Artwork moves with its own cell, keeping its position relative to that cell." };///< The long description of the init command.
    static constexpr const char* const wroteFile                  { "wrote "                                                                                                                                                                                                                                                                                                                                                                            };///< Prefix of the success line of a written file.
};

/**______________________________END OF NAMESPACE______________________________*/
}// namespace text
