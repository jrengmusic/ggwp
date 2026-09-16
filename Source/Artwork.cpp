#include "Artwork.h"

using Element = jam::Document::Element;

static const auto& getShiftedAttributes()
{
    static const auto shiftedAttributes { [] {
        static constexpr int decimalPlaces { 3 };

        auto formatCoordinate = [] (float value)
        {
            return juce::String (value, decimalPlaces).trimCharactersAtEnd ("0").trimCharactersAtEnd (".");
        };

        auto shiftCoordinates = [formatCoordinate] (const Element& element, int moveX, int moveY,
                                                     const std::vector<juce::Identifier>& xKeys,
                                                     const std::vector<juce::Identifier>& yKeys)
        {
            jam::HashMap<juce::Identifier, juce::String> shifted;

            for (auto& key : xKeys)
                shifted.insert ({ key, formatCoordinate (element.get<juce::String> (key, {}).getFloatValue() + static_cast<float> (moveX)) });

            for (auto& key : yKeys)
                shifted.insert ({ key, formatCoordinate (element.get<juce::String> (key, {}).getFloatValue() + static_cast<float> (moveY)) });

            return shifted;
        };

        jam::Function::Map<juce::String, jam::HashMap<juce::Identifier, juce::String>> shiftedAttributes;

        shiftedAttributes.add<const Element&, int, int> (Id::path.toString(),
            [] (const Element& element, int moveX, int moveY)
            {
                auto path { jam::Svg::getElementPath (element) };
                path.applyTransform (juce::AffineTransform::translation (
                    static_cast<float> (moveX), static_cast<float> (moveY)));

                jam::HashMap<juce::Identifier, juce::String> shifted;
                shifted.insert ({ Id::d, jam::Svg::Format::pathToString (path) });
                return shifted;
            });

        shiftedAttributes.add<const Element&, int, int> (Id::rect.toString(),
            [shiftCoordinates] (const Element& element, int moveX, int moveY)
            { return shiftCoordinates (element, moveX, moveY, { Id::x }, { Id::y }); });

        shiftedAttributes.add<const Element&, int, int> (Id::ellipse.toString(),
            [shiftCoordinates] (const Element& element, int moveX, int moveY)
            { return shiftCoordinates (element, moveX, moveY, { Id::cx }, { Id::cy }); });

        shiftedAttributes.add<const Element&, int, int> (Id::circle.toString(),
            [shiftCoordinates] (const Element& element, int moveX, int moveY)
            { return shiftCoordinates (element, moveX, moveY, { Id::cx }, { Id::cy }); });

        shiftedAttributes.add<const Element&, int, int> (Id::line.toString(),
            [shiftCoordinates] (const Element& element, int moveX, int moveY)
            { return shiftCoordinates (element, moveX, moveY, { Id::x1, Id::x2 }, { Id::y1, Id::y2 }); });

        return shiftedAttributes;
    } () };

    return shiftedAttributes;
}

juce::String getArtworkSvg (const Element& artwork, int deltaX, int deltaY, const juce::String& groupId)
{
    static constexpr int artworkDepth { 2 };

    if (deltaX == 0 and deltaY == 0)
        return jam::Xml::getText (artwork, artworkDepth);

    const auto& shiftedAttributes { getShiftedAttributes() };
    const juce::String tag { artwork.id.toString() };

    if (shiftedAttributes.contains (tag))
        return jam::Xml::getText (artwork,
            shiftedAttributes.get (tag, artwork, deltaX, deltaY),
            artworkDepth);

    juce::ConsoleApplication::fail ("glyph group " + groupId + " artwork tag " + tag + " has no shift rule");
}
