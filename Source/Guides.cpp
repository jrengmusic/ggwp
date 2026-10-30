#include "Guides.h"
#include "generated/Generated.h"
#include "Manifest.h"
#include "Artwork.h"
#include "Sheet.h"

using Element = jam::Document::Element;

static juce::String getLineSvg (float fromX, float fromY, float toX, float toY, const juce::Colour& colour)
{
    juce::Path line;
    line.startNewSubPath (fromX, fromY);
    line.lineTo (toX, toY);

    return jam::Svg::Format::stroke (line, colour, hairlineStrokeWidth);
}

static juce::String getLabelSvg (const juce::String& labelColour, const juce::String& glyphText,
                                 juce::Rectangle<float> cellBounds)
{
    static constexpr int labelXOffset { 2 };
    static constexpr int labelYOffset { 8 };
    static const juce::String labelFontSize { "7px" };

    juce::XmlElement label { Id::text.toString() };
    label.setAttribute (Id::x, getCoordinateText (cellBounds.getX() + labelXOffset));
    label.setAttribute (Id::y, getCoordinateText (cellBounds.getY() + labelYOffset));
    label.setAttribute (Id::style, "font-size:" + labelFontSize + ";fill:" + labelColour);
    label.addTextElement (glyphText);

    return childIndent + jam::Xml::getText (*jam::Xml::toDocument (label).getRoot());
}

static juce::String getGuideLinesSvg (const jam::ConfigDocument& manifest, juce::Rectangle<float> cellBounds)
{
    juce::String guideLinesSvg;

    for (auto* guideRow : manifest.getTableRows (Id::guides))
    {
        const auto lineY { cellBounds.getY() + getCellText (manifest, *guideRow, Id::offset).getIntValue() };
        const auto colour { jam::Svg::parseColour (getColourText (manifest, juce::Identifier { getCellText (manifest, *guideRow, Id::key) })) };

        guideLinesSvg << getLineSvg (cellBounds.getX(), lineY, cellBounds.getRight(), lineY, colour);
    }

    return guideLinesSvg;
}

static juce::String getAdvanceLinesSvg (const juce::ValueTree& metrics, const juce::String& glyphText,
                                        juce::Rectangle<float> cellBounds, const juce::Colour& colour)
{
    const double cellWidth { metrics.getProperty (Id::cellWidth) };
    const auto plainAdvance { jam::OpenTypeDocument::getPlainAdvance (metrics) };
    const auto sheetScale { jam::OpenTypeDocument::getSheetScale (metrics) };
    const auto codepointCount { glyphText.length() };
    const auto advance { jam::OpenTypeDocument::getGlyphAdvance (codepointCount, cellBounds.getWidth(), cellWidth, plainAdvance) };

    juce::String advanceLinesSvg;

    for (int unitEdge { 1 }; unitEdge < codepointCount; ++unitEdge)
    {
        const auto edgeX { cellBounds.getX() + static_cast<float> (unitEdge * plainAdvance / sheetScale) };

        advanceLinesSvg << getLineSvg (edgeX, cellBounds.getY(), edgeX, cellBounds.getBottom(), colour);
    }

    const auto advanceX { cellBounds.getX() + static_cast<float> (advance / sheetScale) };

    advanceLinesSvg << getLineSvg (advanceX, cellBounds.getY(), advanceX, cellBounds.getBottom(), colour);

    return advanceLinesSvg;
}

juce::String getCellGuidesSvg (const jam::ConfigDocument& manifest, const juce::ValueTree& metrics, const juce::String& glyphText,
                               juce::Rectangle<float> cellBounds, const juce::String& labelColour, const juce::Colour& advanceColour)
{
    return getLabelSvg (labelColour, glyphText, cellBounds)
         + getGuideLinesSvg (manifest, cellBounds)
         + getAdvanceLinesSvg (metrics, glyphText, cellBounds, advanceColour);
}

const Element* getBackgroundRect (const Element& root)
{
    for (auto* child : root)
        if (child->isTag (Id::rect) and child->contains (Id::fill))
            return child;

    return nullptr;
}

juce::String getBackgroundRectSvg (const Element& backgroundRect, float canvasWidth, float canvasHeight,
                                   const juce::String& backgroundColour)
{
    jam::HashMap<juce::Identifier, juce::String> canvasSize;
    canvasSize.insert ({ Id::width, getCoordinateText (canvasWidth) });
    canvasSize.insert ({ Id::height, getCoordinateText (canvasHeight) });
    canvasSize.insert ({ Id::fill, backgroundColour });

    return jam::Xml::getText (backgroundRect, canvasSize, 1);
}
