#pragma once
#include <JuceHeader.h>

/**
 * @file Guides.h
 * @brief Writes a glyph sheet cell's label and guide-line overlay, and
 *        reads/writes the sheet's own background rect.
 */

/** @brief Stroke width, in Svg user units, shared by every guide line this file draws. */
extern const juce::String hairlineStrokeWidth;

/**
 * @brief Builds one cell's codepoint label plus its guide-line overlay.
 *
 * Writes @p glyphText as an escaped Svg \<text\> label at the cell's top-left,
 * one hairline \<path\> per manifest guides-table row (at that row's offset
 * and colour), and one dashed centreline \<path\> down the cell's middle.
 *
 * @param manifest    The font family manifest, read for its guides table.
 * @param glyphText   The cell's codepoint label text.
 * @param cellBounds  The cell's rectangle within the glyph sheet.
 * @return The cell's label and guide-line Svg markup.
 */
juce::String getCellGuidesSvg (const jam::ConfigDocument& manifest, const juce::String& glyphText,
                               juce::Rectangle<int> cellBounds);

/**
 * @brief Finds the sheet's background rect among @p root's direct children.
 *
 * The background rect is the first \<rect\> child carrying a \c fill attribute.
 *
 * @param root The glyph sheet's root element.
 * @return The background rect element, or nullptr when none is present.
 */
const jam::Document::Element* getBackgroundRect (const jam::Document::Element& root);

/**
 * @brief Serialises the sheet's background rect, resized to the sheet canvas.
 *
 * @param backgroundRect The background rect element, as found by getBackgroundRect().
 * @param canvasWidth    The glyph sheet canvas width to write onto the rect.
 * @param canvasHeight   The glyph sheet canvas height to write onto the rect.
 * @return The background rect's Svg markup, width/height overridden to the canvas size.
 */
juce::String getBackgroundRectSvg (const jam::Document::Element& backgroundRect, int canvasWidth, int canvasHeight);
