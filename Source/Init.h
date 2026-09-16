#pragma once
#include <JuceHeader.h>

/**
 * @file Init.h
 * @brief Entry point for the \--init command line option — creates a font
 *        family's missing manifest and table entries, then reconciles every
 *        declared weight's glyph sheet against the manifest.
 */

/** @brief The command line option that selects the init command. */
extern const juce::String initOption;

/**
 * @brief Handles the \--init command line option.
 *
 * Reads the family name from \--init. Creates the manifest file when it is
 * missing, seeded from the regular weight's glyph sheet when that sheet
 * already exists on disk. Adds a missing glyphs table from the first
 * declared weight whose glyph sheet exists on disk, and a missing guides
 * table with default values.
 * Then, for every weight declared in the manifest's font table, writes that
 * weight's glyph sheet, reconciled against the manifest's \c \#\# glyphs
 * table -- existing cells keep their own width and artwork, missing cells
 * are added, and the sheet is repacked.
 *
 * @param args  The parsed command line arguments; must carry \--init.
 */
void runInitCommand (const juce::ArgumentList& args);
