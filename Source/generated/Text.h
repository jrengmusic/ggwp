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
    static constexpr const char* const failSheetNotFound       { "glyph sheet not found"             };///< A weight's glyph sheet file does not exist.
    static constexpr const char* const failRoundTrip           { "round trip validation failed"      };///< Prefix of every failure of the load-back check.
    static constexpr const char* const failFreeTypeInit        { "FT_Init_FreeType failed"           };///< FreeType could not initialise.
    static constexpr const char* const failFreeTypeFace        { "FT_New_Memory_Face failed"         };///< FreeType could not load the written font.
    static constexpr const char* const failLoadGlyph           { "FT_Load_Glyph failed"              };///< FreeType could not load a glyph.
    static constexpr const char* const failGlyphAdvance        { "glyph advance differs"             };///< The loaded glyph's advance differs from the model.
    static constexpr const char* const failCharIndex           { "character map entry differs"       };///< A codepoint maps to another glyph than the model's.
    static constexpr const char* const failGlyphOutline        { "glyph has no outline contours"     };///< A glyph that the model draws has no contour in the font.
    static constexpr const char* const failGlyphCount          { "glyph count differs"               };///< The font's glyph count differs from the model's.
    static constexpr const char* const failWeightUnknown       { "unknown weight"                    };///< The weight is not a table of the manifest.
    static constexpr const char* const failFontWrite           { "failed to write font file"         };///< The font file could not be written.
    static constexpr const char* const failManifestWrite       { "failed to write manifest file"     };///< The manifest file could not be written.
    static constexpr const char* const failSheetsDirectory     { "failed to create sheets directory" };///< The sheets directory could not be created.
    static constexpr const char* const failSheetWrite          { "failed to write glyph sheet"       };///< A glyph sheet file could not be written.
    static constexpr const char* const failManifestCell        { "manifest table cell missing"       };///< A manifest table row has no cell for a column.
    static constexpr const char* const failMetricsRow          { "manifest metrics row missing"      };///< The metrics table has no cellWidth row.
    static constexpr const char* const failGlyphRowReserved    { "reserved glyph row"                };///< A glyphs row holds the notdef or the space.
    static constexpr const char* const failGlyphRowUnsupported { "unsupported glyph row"             };///< A glyphs row has an unsupported shape.
    static constexpr const char* const failGlyphGroupRect      { "glyph group has no rect cell"      };///< A glyph group of a sheet has no rect cell.
    static constexpr const char* const failArtworkShift        { "artwork tag has no shift rule"     };///< An artwork element has a tag that the sheet cannot shift.
    static constexpr const char* const failSheetXml            { "glyph sheet is not valid XML"      };///< A glyph sheet does not parse.
    static constexpr const char* const failFontFace            { "font cannot be opened"             };///< The font file given to the extract command cannot be opened.
};

/**______________________________END OF NAMESPACE______________________________*/
}// namespace text
