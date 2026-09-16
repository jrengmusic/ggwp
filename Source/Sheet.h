#pragma once
#include <JuceHeader.h>

/**
 * @file Sheet.h
 * @brief Builds a weight's glyph sheet text, reconciled against a font
 *        family manifest's glyphs table.
 */

/**
 * @brief Returns the complete text of a glyph sheet, reconciled against a
 *        manifest's glyphs table.
 *
 * @p sheetText is the existing sheet's text, or empty when no sheet exists
 * yet. The manifest's \c \#\# glyphs table decides which cells exist in the
 * returned sheet: a cell present in both the table and the existing sheet
 * keeps its own width and its artwork; a cell absent from the table is not
 * emitted. The cells repack in table order and the canvas resizes to fit
 * them. The parsed existing sheet is read and never modified.
 *
 * @param manifest  The font family manifest, read for its glyphs and guides tables.
 * @param sheetText The existing sheet's text, or empty when none exists.
 * @return The complete text of the reconciled glyph sheet.
 */
juce::String getSheetSvg (const jam::ConfigDocument& manifest, const juce::String& sheetText);

/**
 * @brief Parses a glyph sheet's Svg text, failing fast on invalid XML.
 *
 * Empty @p sheetText parses to an empty Document (no sheet exists yet) and
 * is not treated as invalid. A non-empty @p sheetText that fails to parse
 * fails the console application, naming @p origin in the message.
 *
 * @param sheetText The glyph sheet's Svg source text, or empty when none exists.
 * @param origin    Identifies the sheet's source (e.g. file path) in the failure message.
 * @return The parsed Document.
 */
jam::XmlDocument getSheetDocument (const juce::String& sheetText, const juce::String& origin);
