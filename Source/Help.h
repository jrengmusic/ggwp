#pragma once
#include <JuceHeader.h>

/**
 * @file Help.h
 * @brief Entry point for the \--help command line option — prints the GGWP guide.
 */

/**
 * @brief Prints the HELP.md text to stdout.
 *
 * @param helpText The HELP.md text, printed verbatim on `--help` and when
 *                 no manifest is found.
 */
inline void printHelp (const juce::String& helpText)
{
    std::cout << helpText;
}
