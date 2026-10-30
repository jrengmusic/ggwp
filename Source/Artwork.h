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
 * Zero in both @p deltaX and @p deltaY writes @p artwork unchanged via
 * jam::Xml::getText(). Otherwise, the element's tag selects an entry of the
 * shifted-attribute table: path translates its \c d data, rect shifts
 * \c x and \c y, ellipse and circle shift \c cx and \c cy, and line shifts both
 * end points. The element is written with those attributes overridden. A tag
 * absent from the table fails the console application, naming @p groupId and
 * the tag in the message.
 *
 * @param artwork The artwork element to serialise.
 * @param deltaX  Horizontal shift in glyph-sheet units.
 * @param deltaY  Vertical shift in glyph-sheet units.
 * @param groupId The glyph group's id, named in the failure message when
 *                @p artwork's tag is absent from the table.
 * @return The artwork element's Svg markup, coordinates shifted by the delta.
 */
juce::String getArtworkSvg (const jam::Document::Element& artwork, float deltaX, float deltaY, const juce::String& groupId);

/**
 * @brief Formats a sheet coordinate as the shortest text that holds three decimal places.
 *
 * Trailing zeros and a trailing decimal point are trimmed, so a whole value
 * writes without a fraction.
 *
 * @param coordinate The coordinate, in glyph-sheet units.
 * @return The coordinate text.
 */
juce::String getCoordinateText (float coordinate);
