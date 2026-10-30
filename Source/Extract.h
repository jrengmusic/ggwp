#pragma once
#include <JuceHeader.h>

/**
 * @file Extract.h
 * @brief Entry point for the \--extract command line option -- writes a
 *        weight's glyph sheet from an existing font.
 */

/** @brief Returns the command line option that selects the extract command. */
const juce::String& getExtractOption();

/**
 * @brief Handles the \--extract command line option.
 *
 * Reads the font named by \--extract and the manifest named by \--manifest. Every
 * outline and advance is scaled from the font's units per em to the sheet's units
 * per em, and drawn into a raw sheet: one cell per character above the space, plus
 * one cell per ligature that shaping finds. A manifest without a glyphs table gains
 * one built from the raw sheet, and is written back. The sheet is then laid out from
 * the manifest's glyphs table, so only the cells that the table lists are written, to
 * the path that the weight table named by \--weight gives. The manifest metrics are
 * read and never changed.
 *
 * @param args  The parsed command line arguments; must carry \--extract, \--manifest and \--weight.
 */
void runExtractCommand (const juce::ArgumentList& args);

/**
 * @brief Registers font bytes with a typeface and returns the entry index.
 *
 * The build command shares this function to load the font that it just wrote, so
 * both commands read a font through one path.
 *
 * @param typeface  The typeface that takes the registration.
 * @param fontBytes The font file's bytes.
 * @param fontFile  The font file; named in the failure message.
 * @return The index of the registered entry in @p typeface.
 */
int getFontEntry (jam::Typeface& typeface, const juce::MemoryBlock& fontBytes, const juce::File& fontFile);
