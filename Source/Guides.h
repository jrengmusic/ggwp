#pragma once
#include <JuceHeader.h>

/**
 * @file Guides.h
 * @brief Writes a glyph sheet cell's label and guide-line overlay, and
 *        reads and writes the sheet's own background rect.
 */

/** @brief Stroke width, in Svg user units, shared by every guide line this file draws. */
static constexpr float hairlineStrokeWidth { 0.5f };

/**
 * @brief Builds one cell's codepoint label plus its guide-line overlay.
 *
 * Writes @p glyphText as an escaped Svg \<text\> label at the cell's top-left,
 * one horizontal hairline \<path\> per manifest guides-table row (at that row's
 * offset from the cell top, in the colour-table colour of the row's key), and
 * one vertical hairline \<path\> at each unit-cell edge inside the glyph and at
 * the glyph's advance. The edge and advance positions follow the build's own
 * advance rule, applied to @p cellBounds and @p metrics.
 *
 * @param manifest       The font family manifest, read for its guides and colour tables.
 * @param metrics        The manifest's metrics value tree, read for the cell width, plain advance and sheet scale.
 * @param glyphText      The cell's codepoint label text.
 * @param cellBounds     The cell's rectangle within the glyph sheet.
 * @param labelColour    The label's fill colour, as colour-table text.
 * @param advanceColour  The colour of the vertical unit-cell edge and advance lines.
 * @return The cell's label and guide-line Svg markup.
 */
juce::String getCellGuidesSvg (const jam::ConfigDocument& manifest, const juce::ValueTree& metrics, const juce::String& glyphText,
                               juce::Rectangle<float> cellBounds, const juce::String& labelColour, const juce::Colour& advanceColour);

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
 * @param backgroundRect   The background rect element, as found by getBackgroundRect().
 * @param canvasWidth      The glyph sheet canvas width to write onto the rect.
 * @param canvasHeight     The glyph sheet canvas height to write onto the rect.
 * @param backgroundColour The fill colour to write onto the rect.
 * @return The background rect's Svg markup, width, height and fill overridden.
 */
juce::String getBackgroundRectSvg (const jam::Document::Element& backgroundRect, float canvasWidth, float canvasHeight,
                                   const juce::String& backgroundColour);
