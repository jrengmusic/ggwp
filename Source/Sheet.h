#pragma once
#include <JuceHeader.h>

/**
 * @file Sheet.h
 * @brief Builds a weight's glyph sheet text, reconciled against a font
 *        family manifest's glyphs table, and writes it to disk.
 */

/**
 * @brief Returns the complete text of a glyph sheet, reconciled against a
 *        manifest's glyphs table.
 *
 * @p sheetText is the existing sheet's text, or empty when no sheet exists
 * yet. The manifest's \c \#\# glyphs table decides which cells exist in the
 * returned sheet: a cell present in both the table and the existing sheet
 * keeps its own width and its artwork, shifted to follow the cell; a cell
 * absent from the table is not emitted, and its artwork goes with it. A cell
 * new to the sheet takes the width of its codepoint count in plain cells. The
 * cells repack in table order, in rows grouped by codepoint count, and the
 * canvas resizes to fit them. Top-level elements that are neither a glyph
 * group nor the guides layer pass through, and the existing background rect
 * is resized to the canvas. The command fails on a table row that holds a
 * reserved codepoint or more than three codepoints. The parsed existing sheet
 * is read and never modified.
 *
 * @param manifest  The font family manifest, read for its glyphs, guides, colour and metrics tables.
 * @param sheetText The existing sheet's text, or empty when none exists.
 * @return The complete text of the reconciled glyph sheet.
 */
juce::String getSheetSvg (const jam::ConfigDocument& manifest, const juce::String& sheetText);

/**
 * @brief Parses a glyph sheet's Svg text, failing fast on invalid XML.
 *
 * Empty @p sheetText returns a new empty jam::XmlDocument (no sheet exists yet)
 * and is not treated as invalid. A non-empty @p sheetText returns the document
 * that jam::Xml::parse gives. Invalid text fails the console application with
 * text::Diagnostics::failSheetXml, @p origin, and the jam::XmlDocument::isValid
 * message, joined by Id::diagnosticSeparator.
 *
 * @param sheetText The glyph sheet's Svg source text, or empty when none exists.
 * @param origin    Identifies the sheet's source (e.g. file path) in the failure message.
 * @return The parsed Document.
 */
std::unique_ptr<jam::XmlDocument> getSheetDocument (const juce::String& sheetText, const juce::String& origin);

/** @brief The codepoint count of a single glyph's cell. */
static constexpr int singleCodepointCount { 1 };

/** @brief The codepoint count of a two-character ligature's cell. */
static constexpr int doubleCodepointCount { 2 };

/** @brief The codepoint count of a three-character ligature's cell, the largest the sheet lays out. */
static constexpr int tripleCodepointCount { 3 };

/**
 * @brief Returns the width of a ligature's cell, in glyph-sheet units.
 *
 * The cell spans one plain cell for each codepoint, plus one gutter between
 * each pair of neighbouring plain cells.
 *
 * @param codepointCount The number of codepoints in the ligature.
 * @param cellWidth      The width of one plain cell, from the manifest metrics.
 * @return The ligature cell's width.
 */
double getLigatureCellWidth (int codepointCount, double cellWidth);

/**
 * @brief Writes a glyph sheet's text to its file, creating missing parent directories.
 *
 * The command fails when the directory cannot be created or the file cannot be replaced.
 *
 * @param sheetFile The file that receives the sheet text.
 * @param sheetText The complete sheet text.
 */
void writeSheetFile (const juce::File& sheetFile, const juce::String& sheetText);

/** @brief The Svg namespace written onto every generated sheet root. */
extern const juce::String svgNamespace;

/** @brief The indent of an element nested two levels deep in a generated sheet, shared by cell and guide markup. */
extern const juce::String childIndent;
