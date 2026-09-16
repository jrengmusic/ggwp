#pragma once
#include <JuceHeader.h>

/**
 * @file Artwork.h
 * @brief Writes a single glyph's artwork element back out as Svg markup,
 *        shifting its coordinates when the glyph moved during repack.
 */

/**
 * @brief Serialises @p artwork as Svg markup, shifted by (deltaX, deltaY).
 *
 * @p deltaX == 0 and @p deltaY == 0 writes @p artwork unchanged via
 * jam::Xml::getText(). Otherwise, the element's tag (path/rect/ellipse/
 * circle/line) selects a shift rule that recomputes its coordinate
 * attributes for the delta and writes the element with those attributes
 * overridden. A tag with no shift rule fails the console application,
 * naming @p groupId in the message.
 *
 * @param artwork The artwork element to serialise.
 * @param deltaX  Horizontal shift in glyph-sheet units.
 * @param deltaY  Vertical shift in glyph-sheet units.
 * @param groupId The glyph group's id, named in the failure message when
 *                @p artwork's tag has no shift rule.
 * @return The artwork element's Svg markup, coordinates shifted by the delta.
 */
juce::String getArtworkSvg (const jam::Document::Element& artwork, int deltaX, int deltaY, const juce::String& groupId);
