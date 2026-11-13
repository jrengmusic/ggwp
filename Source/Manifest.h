#pragma once
#include <JuceHeader.h>

/**
 * @file Manifest.h
 * @brief Builds, reads, validates and writes a font family's manifest document --
 *        the markdown tables that declare metrics, font metadata, glyphs, guides,
 *        colours and each weight's own table.
 */

/**
 * @brief Resolves a weight's glyph sheet file from its manifest-relative path.
 *
 * @param manifestFile The manifest file; its parent directory is the one
 *                     path rule this function applies.
 * @param weight       The weight's ValueTree, holding its property keyed by Extensions::svg.
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
 * @brief Builds the manifest of a new font family from the embedded default manifest.
 *
 * The text is the bundled markdown file files::ggwpManifest, with the family and
 * PostScript name base written into its placeholders by jam::Format::replaceholder.
 * The command fails when the text does not parse as a config document, naming the
 * file name.
 *
 * @param familyName     The family name written into the default manifest.
 * @param postScriptBase The PostScript name base written into the default manifest.
 * @return The parsed manifest document.
 */
jam::ConfigDocument getDefaultManifest (const juce::String& familyName, const juce::String& postScriptBase);

/**
 * @brief Adds a table, with its heading, to the manifest from the default manifest.
 *
 * Gives a manifest that predates a table the rows that the default manifest holds
 * for it.
 *
 * @param manifest The manifest document that gains the table.
 * @param tableId  The identifier of the table to copy from the default manifest.
 */
void addDefaultTable (jam::ConfigDocument& manifest, const juce::Identifier& tableId);

/**
 * @brief Adds the glyphs heading and table to the manifest document.
 *
 * One row is added for each glyph group of @p sheetRoot, in sheet order, when
 * @p sheetRoot is given. The default printable ASCII range, U+0021 through U+007E,
 * is added when @p sheetRoot is nullptr.
 *
 * @param document  The manifest document to add the section to.
 * @param sheetRoot The root element of an existing glyph sheet, or
 *                  nullptr to build the default glyphs table.
 */
void addGlyphs (jam::ConfigDocument& document, const jam::Document::Element* sheetRoot);

/**
 * @brief Reads a table cell's raw text.
 *
 * The raw text is the cell's source text, so one reading works for parsed cells
 * and for cells added in memory. The command fails when the row has no cell in
 * the column, naming the table and the column.
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

/**
 * @brief Reads one colour from the manifest's colour table.
 *
 * The command fails when the colour table has no row for @p key.
 *
 * @param manifest The manifest document holding the colour table.
 * @param key      The colour row's key.
 * @return The row's colour text.
 */
juce::String getColourText (const jam::ConfigDocument& manifest, const juce::Identifier& key);

/**
 * @brief Parses a manifest file and validates it against the manifest schema.
 *
 * The command fails when the file does not parse as a valid config document, naming
 * the file's full path and the validation message.
 *
 * @param manifestFile The manifest file to read.
 * @return The parsed, valid manifest document.
 */
jam::ConfigDocument getValidatedManifest (const juce::File& manifestFile);

/**
 * @brief Serialises a manifest document to its markdown file.
 *
 * The command fails when the file cannot be replaced.
 *
 * @param manifestFile The file that receives the manifest text.
 * @param document     The manifest document to serialise.
 */
void writeManifestFile (const juce::File& manifestFile, const jam::ConfigDocument& document);

/**
 * @brief Reads the \--weight option and checks it against the manifest's weight tables.
 *
 * The command fails when the option is missing, or when the manifest declares no
 * weight table of that name. The failure names the manifest file.
 *
 * @param args         The parsed command line arguments; must carry \--weight.
 * @param values       The manifest's font value tree, holding one child for each weight table.
 * @param manifestFile The manifest file; named in the failure message.
 * @return The identifier of the selected weight.
 */
juce::Identifier getValidatedWeight (const juce::ArgumentList& args, const juce::ValueTree& values, const juce::File& manifestFile);
