#pragma once
#include <JuceHeader.h>

/**
 * @file Init.h
 * @brief Entry point for the \--init command line option — writes a new
 *        font family's manifest and glyph sheet.
 */

/** @brief The command line option that selects the init command. */
inline const juce::String initOption { "--init" };

/**
 * @brief Handles the \--init command line option.
 *
 * Reads the family name from \--init, builds a manifest document, then
 * writes the manifest file and its glyph sheet into the current working
 * directory.
 *
 * @param args  The parsed command line arguments; must carry \--init.
 */
void runInitCommand (const juce::ArgumentList& args);
