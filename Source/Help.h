#pragma once
#include <JuceHeader.h>
#include "generated/Generated.h"

/**
 * @file Help.h
 * @brief Renders the bundled HELP.md guide to the terminal.
 */

/**
 * @brief Renders the guide text to stdout as styled terminal output.
 *
 * The text is parsed as markdown and laid out once, at the column width of the
 * markdown style, so the guide wraps to the style's own measure rather than to the
 * terminal's. The viewed component is painted through a terminal graphics context
 * into a terminal buffer sized to that width and to the component's height. The
 * buffer then writes the frame as VT bytes to stdout.
 *
 * @pre The JUCE GUI initialiser scope is open.
 *
 * @param helpText The HELP.md text, parsed as markdown.
 */
void printHelp (const juce::String& helpText);
