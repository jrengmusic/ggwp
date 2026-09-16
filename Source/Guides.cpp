#include "Guides.h"
#include "Manifest.h"

using Element = jam::Document::Element;

static const juce::String childIndent { jam::Xml::getIndent (2) };
const juce::String hairlineStrokeWidth { "0.5" };

juce::String getCellGuidesSvg (const jam::ConfigDocument& manifest, const juce::String& glyphText,
                               juce::Rectangle<int> cellBounds)
{
    static constexpr int labelXOffset { 2 };
    static constexpr int labelYOffset { 8 };
    static const juce::String labelColour { "#646464" };
    static const juce::String centrelineColour { "#444444" };
    static const juce::String labelFontSize { "7px" };
    static const juce::String centrelineDashPattern { "2,3" };

    const auto x { cellBounds.getX() };
    const auto y { cellBounds.getY() };
    const auto width { cellBounds.getWidth() };

    const auto* utf8Text { glyphText.toRawUTF8() };
    const auto utf8Length { glyphText.getNumBytesAsUTF8() };

    std::string escaped;
    jam::Format::escape (utf8Text, utf8Text + utf8Length, std::back_inserter (escaped));

    juce::String guidesSvg;
    guidesSvg << childIndent << "<text x=\"" << (x + labelXOffset) << "\" y=\"" << (y + labelYOffset)
              << "\" style=\"font-size:" << labelFontSize << ";fill:" << labelColour << "\">" << escaped << "</text>\n";

    for (auto* guideRow : manifest.getTableRows (Id::guides))
    {
        const auto offset { getCellText (manifest, *guideRow, Id::offset).getIntValue() };
        const auto colour { getCellText (manifest, *guideRow, Id::colour) };
        const auto lineY { y + offset };

        guidesSvg << childIndent << "<path d=\"M " << x << "," << lineY << " L " << (x + width) << "," << lineY
                  << "\" style=\"fill:none;stroke:" << colour << ";stroke-width:" << hairlineStrokeWidth << "\"/>\n";
    }

    const auto centreX { x + width / 2 };

    guidesSvg << childIndent << "<path d=\"M " << centreX << "," << y << " L " << centreX << ","
              << cellBounds.getBottom()
              << "\" style=\"fill:none;stroke:" << centrelineColour
              << ";stroke-width:" << hairlineStrokeWidth << ";stroke-dasharray:" << centrelineDashPattern << "\"/>\n";

    return guidesSvg;
}

const Element* getBackgroundRect (const Element& root)
{
    for (auto* child : root)
        if (child->isTag (Id::rect) and child->contains (Id::fill))
            return child;

    return nullptr;
}

juce::String getBackgroundRectSvg (const Element& backgroundRect, int canvasWidth, int canvasHeight)
{
    jam::HashMap<juce::Identifier, juce::String> canvasSize;
    canvasSize.insert ({ Id::width, juce::String (canvasWidth) });
    canvasSize.insert ({ Id::height, juce::String (canvasHeight) });

    return jam::Xml::getText (backgroundRect, canvasSize, 1);
}
