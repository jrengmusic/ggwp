#pragma once
#include <JuceHeader.h>

/**
 * @file Manifest.h
 * @brief Builds and reads a font family's manifest document -- the markdown
 *        tables that declare metrics, font metadata, glyphs, guides, and
 *        each weight's own table.
 */

/** @brief Path, relative to the manifest file, to the regular weight's glyph sheet. */
extern const juce::String regularSheetPath;

/**
 * @brief Resolves a weight's glyph sheet file from its manifest-relative path.
 *
 * @param manifestFile The manifest file; its parent directory is the one
 *                     path rule this function applies.
 * @param weight       The weight's ValueTree, holding its Id::svg property.
 * @return The manifest's parent directory joined with the weight's svg path.
 */
juce::File getWeightSheetFile (const juce::File& manifestFile, const juce::ValueTree& weight);

/**
 * @brief Derives the manifest file name from a font family name.
 *
 * @param familyName The font family name.
 * @return The family name, lowercased with spaces replaced by hyphens, and a ".md" extension.
 */
juce::String getManifestFileName (const juce::String& familyName);

/**
 * @brief Derives the PostScript name base from a font family name.
 *
 * @param familyName The font family name.
 * @return The family name with all spaces removed.
 */
juce::String getPostScriptBase (const juce::String& familyName);

/**
 * @brief Adds the guides heading and table to the manifest document.
 *
 * @param document The manifest document to add the section to.
 */
void addGuides (jam::ConfigDocument& document);

/**
 * @brief Adds the glyphs heading and table to the manifest document.
 *
 * The glyphs table is read from @p sheetRoot when given, or built from
 * the default printable ASCII range when @p sheetRoot is nullptr.
 *
 * @param document  The manifest document to add the section to.
 * @param sheetRoot The root element of an existing glyph sheet, or
 *                   nullptr to build the default glyphs table.
 */
void addGlyphs (jam::ConfigDocument& document, const jam::Document::Element* sheetRoot);

/**
 * @brief Builds a new manifest document with default metrics and font
 *        tables, and a glyphs table.
 *
 * The glyphs table is read from @p sheetRoot when given, or built from
 * the default printable ASCII range when @p sheetRoot is nullptr.
 *
 * @param familyName     The font family name.
 * @param postScriptBase The PostScript name base.
 * @param sheetRoot      The root element of an existing glyph sheet, or
 *                        nullptr to build the default glyphs table.
 * @return The built manifest document.
 */
jam::ConfigDocument getManifestDocument (const juce::String& familyName, const juce::String& postScriptBase,
                                         const jam::Document::Element* sheetRoot);

/**
 * @brief Reads a table cell's raw text.
 *
 * Reads Id::rawText directly, instead of through
 * jam::ConfigDocument::getTableValue(), which resolves through
 * getAllSubText(). Both cell-construction paths -- the markdown parser's
 * addTableCell() and the in-memory addCell() -- write Id::rawText
 * unconditionally, so every cell carries it as its exact source text.
 * getAllSubText() instead walks a cell's rendered child elements, which
 * addCell() never adds; reading Id::rawText gives one reading that works
 * for every manifest cell.
 *
 * @param document The manifest document the row belongs to.
 * @param row      The table row holding the cell.
 * @param columnId The identifier of the column to read.
 * @return The cell's raw text.
 */
juce::String getCellText (const jam::ConfigDocument& document, const jam::Document::Element& row, const juce::Identifier& columnId);

/**
 * @brief Reads a glyphs-table row's codepoints, stripped of their backtick
 *        enclosure.
 *
 * A cell holding a single backtick is stored bare, since a backtick cannot
 * enclose itself.
 *
 * @param document The manifest document the row belongs to.
 * @param row      The glyphs-table row to read.
 * @return The row's glyph text, without its enclosing backticks.
 */
juce::String getGlyphText (const jam::ConfigDocument& document, const jam::Document::Element& row);
