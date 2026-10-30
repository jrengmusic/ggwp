#pragma once
#include <JuceHeader.h>

/**
 * @file Build.h
 * @brief Entry point for the \--build command line option -- writes a weight's
 *        font from a manifest and a glyph sheet, then validates the written font.
 */

/** @brief Returns the command line option that selects the build command. */
const juce::String& getBuildOption();

/**
 * @brief Handles the \--build command line option.
 *
 * Reads the manifest named by \--build, the weight table named by \--weight and
 * that weight's glyph sheet, and writes the font file named by \--out, relative
 * to the current directory. The written bytes are then loaded back through
 * FreeType and checked against the document that produced them, so a font that
 * does not round-trip fails the command instead of reaching the caller.
 *
 * @param args  The parsed command line arguments; must carry \--build, \--weight and \--out.
 */
void runBuildCommand (const juce::ArgumentList& args);
