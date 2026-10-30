#include "Artwork.h"
#include "generated/Generated.h"

using Element = jam::Document::Element;

juce::String getCoordinateText (float coordinate)
{
    static constexpr int decimalPlaces { 3 };

    return juce::String (coordinate, decimalPlaces).trimCharactersAtEnd ("0").trimCharactersAtEnd (".");
}

static const auto& getShiftedAttributes()
{
    static const auto shiftedAttributes { [] {
        jam::HashMap<juce::Identifier, std::vector<std::pair<juce::Identifier, juce::Identifier>>> coordinateKeys;

        coordinateKeys.insert ({ Id::rect, { { Id::x, Id::y } } });
        coordinateKeys.insert ({ Id::ellipse, { { Id::cx, Id::cy } } });
        coordinateKeys.insert ({ Id::circle, { { Id::cx, Id::cy } } });
        coordinateKeys.insert ({ Id::line, { { Id::x1, Id::y1 }, { Id::x2, Id::y2 } } });

        jam::Function::Map<juce::Identifier, jam::HashMap<juce::Identifier, juce::String>> shiftedAttributes;

        shiftedAttributes.add<const Element&, float, float> (Id::path,
            [] (const Element& element, float moveX, float moveY)
            {
                auto path { jam::Svg::getElementPath (element) };
                path.applyTransform (juce::AffineTransform::translation (moveX, moveY));

                jam::HashMap<juce::Identifier, juce::String> shifted;
                shifted.insert ({ Id::d, jam::Svg::Format::pathToString (path) });
                return shifted;
            });

        for (const auto& [tag, keyPairs] : coordinateKeys)
            shiftedAttributes.add<const Element&, float, float> (tag,
                [coordinatePairs = keyPairs] (const Element& element, float moveX, float moveY)
                {
                    jam::HashMap<juce::Identifier, juce::String> shifted;

                    for (const auto& [xKey, yKey] : coordinatePairs)
                    {
                        shifted.insert ({ xKey, getCoordinateText (element.get<juce::String> (xKey, {}).getFloatValue() + moveX) });
                        shifted.insert ({ yKey, getCoordinateText (element.get<juce::String> (yKey, {}).getFloatValue() + moveY) });
                    }

                    return shifted;
                });

        return shiftedAttributes;
    } () };

    return shiftedAttributes;
}

juce::String getArtworkSvg (const Element& artwork, float deltaX, float deltaY, const juce::String& groupId)
{
    static constexpr int artworkDepth { 2 };

    if (deltaX == 0.0f and deltaY == 0.0f)
        return jam::Xml::getText (artwork, artworkDepth);

    const auto& shiftedAttributes { getShiftedAttributes() };

    if (shiftedAttributes.contains (artwork.id))
        return jam::Xml::getText (artwork,
            shiftedAttributes.get (artwork.id, artwork, deltaX, deltaY),
            artworkDepth);

    juce::ConsoleApplication::fail (text::Diagnostics::failArtworkShift + Id::diagnosticSeparator + groupId
                                    + Id::diagnosticSeparator + artwork.id.toString());
}
