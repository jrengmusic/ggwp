#pragma once
#include <JuceHeader.h>

/**
 * @file Init.h
 * @brief Entry point for the \--init command line option -- creates a font
 *        family's missing manifest and tables, then reconciles every
 *        declared weight's glyph sheet against the manifest.
 */

/** @brief Returns the command line option that selects the init command. */
const juce::String& getInitOption();

/**
 * @brief Handles the \--init command line option.
 *
 * Reads the family name from \--init. The manifest file name derives from the
 * family name and lands in the current directory. When the file exists, it is
 * read and validated; otherwise the default manifest for the family is the
 * starting document.
 *
 * Three tables are added when missing. The glyphs table is built from the first
 * declared weight whose glyph sheet exists on disk, or from the default printable
 * ASCII range when no sheet exists. The guides table and the colour table take
 * their default rows. The manifest is written only when it gained a table.
 *
 * Then, for every weight declared in the manifest's font table that has a glyph
 * sheet path, the weight's glyph sheet is written, reconciled against the glyphs
 * table. A weight without a sheet on disk gets one.
 *
 * @param args  The parsed command line arguments; must carry \--init.
 */
void runInitCommand (const juce::ArgumentList& args);
