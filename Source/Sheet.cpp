#include "Sheet.h"
#include "generated/Generated.h"
#include "Manifest.h"
#include "Artwork.h"
#include "Guides.h"

using Element = jam::Document::Element;

static constexpr int sheetColumns { 10 };
static constexpr int sheetMarginTop { 70 };
static constexpr int sheetMarginLeft { 40 };
static constexpr int sheetMarginRight { 40 };
static constexpr int sheetCellHeight { 243 };
static constexpr int sheetGutter { 10 };
static constexpr int rowStep { sheetCellHeight + sheetGutter };
static constexpr int singleGlyphSlot { 0 };
static constexpr int doubleGlyphSlot { 1 };
static constexpr int tripleGlyphSlot { 2 };

const juce::String svgNamespace { "http://www.w3.org/2000/svg" };

static const juce::String groupIndent { jam::Xml::getIndent (1) };
const juce::String childIndent { jam::Xml::getIndent (2) };

static juce::String getGroupId (const juce::String& text)
{
    jam::Document::Numbers codepoints;

    for (auto codepoint : text)
        codepoints.add (static_cast<double> (codepoint));

    return jam::OpenTypeDocument::getGlyphId (codepoints);
}

static int getRowCount (int itemCount, int columnsPerRow)
{
    return (itemCount + columnsPerRow - 1) / columnsPerRow;
}

static int getLigatureColumns (int codepointCount)
{
    return sheetColumns / codepointCount;
}

double getLigatureCellWidth (int codepointCount, double cellWidth)
{
    return cellWidth * codepointCount + sheetGutter * (codepointCount - 1);
}

void writeSheetFile (const juce::File& sheetFile, const juce::String& sheetText)
{
    if (not sheetFile.getParentDirectory().createDirectory().wasOk())
        juce::ConsoleApplication::fail (text::Diagnostics::failSheetsDirectory + Id::diagnosticSeparator
                                        + sheetFile.getParentDirectory().getFullPathName());

    if (not sheetFile.replaceWithText (sheetText))
        juce::ConsoleApplication::fail (text::Diagnostics::failSheetWrite + Id::diagnosticSeparator + sheetFile.getFullPathName());
}

static int getCanvasHeight (int totalRows)
{
    static constexpr int sheetMarginBottom { 20 };

    return sheetMarginTop + totalRows * rowStep - sheetGutter + sheetMarginBottom;
}

static const Element* getCellGroup (const Element& root, const juce::String& groupId)
{
    const auto targetCodepoints { jam::OpenTypeDocument::getCodepoints (groupId) };

    for (auto* child : root)
        if (child->contains (Id::id))
        {
            const auto childCodepoints { jam::OpenTypeDocument::getCodepoints (*child->get<juce::String> (Id::id)) };

            if (std::equal (childCodepoints.begin(), childCodepoints.end(),
                            targetCodepoints.begin(), targetCodepoints.end()))
                return child;
        }

    return nullptr;
}

static std::array<int, tripleCodepointCount> getGlyphRowCounts (const jam::ConfigDocument& manifest)
{
    std::array<int, tripleCodepointCount> rowsByFamily {};

    for (auto* row : manifest.getTableRows (Id::glyphs))
    {
        const auto text { getGlyphText (manifest, *row) };
        const auto codepointCount { text.length() };
        const auto isSupportedCodepointCount { codepointCount >= singleCodepointCount and codepointCount <= tripleCodepointCount };

        if (isSupportedCodepointCount)
        {
            const auto isReservedCodepoint { codepointCount == singleCodepointCount
                                             and (static_cast<int> (text[0]) == jam::OpenTypeDocument::notdefCodepoint
                                                  or static_cast<int> (text[0]) == jam::OpenTypeDocument::spaceCodepoint) };

            if (isReservedCodepoint)
                juce::ConsoleApplication::fail (text::Diagnostics::failGlyphRowReserved + Id::diagnosticSeparator + text);

            ++rowsByFamily.at (codepointCount - singleCodepointCount);
        }
        else
        {
            juce::ConsoleApplication::fail (text::Diagnostics::failGlyphRowUnsupported + Id::diagnosticSeparator + text);
        }
    }

    return rowsByFamily;
}

static std::array<int, tripleCodepointCount> getRowOffsets (const std::array<int, tripleCodepointCount>& glyphRowCounts)
{
    const auto charRows { getRowCount (glyphRowCounts.at (singleGlyphSlot), sheetColumns) };
    const auto doubleRows { getRowCount (glyphRowCounts.at (doubleGlyphSlot), getLigatureColumns (doubleCodepointCount)) };

    return { 0, charRows, charRows + doubleRows };
}

static int getTotalRowCount (const std::array<int, tripleCodepointCount>& glyphRowCounts)
{
    return getRowOffsets (glyphRowCounts).at (tripleGlyphSlot)
         + getRowCount (glyphRowCounts.at (tripleGlyphSlot), getLigatureColumns (tripleCodepointCount));
}

static const Element& getCellRect (const Element& group)
{
    auto* cell { group.getChildByID (Id::rect) };

    if (cell == nullptr)
        juce::ConsoleApplication::fail (text::Diagnostics::failGlyphGroupRect + Id::diagnosticSeparator
                                        + group.get<juce::String> (Id::id, {}));

    return *cell;
}

static float getCellWidth (const Element* oldRect, int codepointCount, double cellWidth)
{
    if (oldRect != nullptr)
        return oldRect->get<juce::String> (Id::width, {}).getFloatValue();

    return static_cast<float> (getLigatureCellWidth (codepointCount, cellWidth));
}

static juce::Rectangle<float> getCellBounds (int codepointCount, int placedCellCount, int firstRow, float previousRight, float width)
{
    const auto columns { getLigatureColumns (codepointCount) };
    const auto row { firstRow + placedCellCount / columns };
    const auto x { placedCellCount % columns == 0 ? static_cast<float> (sheetMarginLeft)
                                                  : previousRight + static_cast<float> (sheetGutter) };

    return { x, static_cast<float> (sheetMarginTop + row * rowStep), width, static_cast<float> (sheetCellHeight) };
}

static juce::String getCellSvg (const Element* oldGroup, const juce::String& groupId, juce::Rectangle<float> cellBounds,
                                const juce::String& cellColour, const juce::String& glyphColour)
{
    juce::XmlElement cell { Id::rect.toString() };
    cell.setAttribute (Id::x, getCoordinateText (cellBounds.getX()));
    cell.setAttribute (Id::y, getCoordinateText (cellBounds.getY()));
    cell.setAttribute (Id::width, getCoordinateText (cellBounds.getWidth()));
    cell.setAttribute (Id::height, getCoordinateText (cellBounds.getHeight()));
    cell.setAttribute (Id::style, "fill:none;stroke:" + cellColour + ";stroke-width:" + juce::String (hairlineStrokeWidth));

    juce::String cellSvg;
    cellSvg << groupIndent << "<g id=\"" << groupId << "\" fill=\"" << glyphColour << "\">\n";
    cellSvg << childIndent << jam::Xml::getText (*jam::Xml::toDocument (cell).getRoot());

    if (oldGroup != nullptr)
    {
        const auto& oldRect { getCellRect (*oldGroup) };
        const auto deltaX { cellBounds.getX() - oldRect.get<juce::String> (Id::x, {}).getFloatValue() };
        const auto deltaY { cellBounds.getY() - oldRect.get<juce::String> (Id::y, {}).getFloatValue() };

        const auto oldGroupId { oldGroup->get<juce::String> (Id::id, {}) };

        for (auto* artwork : *oldGroup)
            if (artwork != &oldRect)
                cellSvg << getArtworkSvg (*artwork, deltaX, deltaY, oldGroupId);
    }

    cellSvg << groupIndent << "</g>\n";
    return cellSvg;
}

static juce::String getDocumentSvg (const Element* backgroundRect, float canvasWidth, float canvasHeight,
                                    const juce::String& guidesSvg, const juce::String& cellsSvg,
                                    const juce::String& passthroughSvg, const juce::String& backgroundColour)
{
    juce::String sheetSvg;
    sheetSvg << "<svg " << Id::xmlns.toString() << "=\"" << svgNamespace << "\" width=\"" << getCoordinateText (canvasWidth)
             << "\" height=\"" << getCoordinateText (canvasHeight) << "\">\n";

    if (backgroundRect != nullptr)
    {
        sheetSvg << getBackgroundRectSvg (*backgroundRect, canvasWidth, canvasHeight, backgroundColour);
    }
    else
    {
        const juce::XmlElement background { Id::rect.toString() };

        sheetSvg << getBackgroundRectSvg (*jam::Xml::toDocument (background).getRoot(), canvasWidth, canvasHeight, backgroundColour);
    }

    sheetSvg << groupIndent << "<g id=\"" << Id::guides.toString() << "\">\n" << guidesSvg << groupIndent << "</g>\n";
    sheetSvg << cellsSvg;
    sheetSvg << passthroughSvg;
    sheetSvg << "</svg>\n";

    return sheetSvg;
}

static juce::String getPassthroughSvg (const Element& root, const Element* backgroundRect)
{
    const auto glyphGroups { jam::OpenTypeDocument::getGlyphGroups (root) };

    juce::String passthroughSvg;

    for (auto* child : root)
        if (child != backgroundRect)
            if (not glyphGroups.contains (child))
                if (not child->hasProperty (Id::id, Id::guides))
                    passthroughSvg << jam::Xml::getText (*child, 1);

    return passthroughSvg;
}

std::unique_ptr<jam::XmlDocument> getSheetDocument (const juce::String& sheetText, const juce::String& origin)
{
    if (sheetText.isEmpty())
        return std::make_unique<jam::XmlDocument>();

    if (auto sheet { jam::Xml::parse (sheetText) }; sheet != nullptr)
        return sheet;

    juce::ConsoleApplication::fail (text::Diagnostics::failSheetXml + Id::diagnosticSeparator + origin + Id::diagnosticSeparator + jam::XmlDocument::isValid (sheetText).getErrorMessage());
}

static jam::Array<juce::Rectangle<float>> getCellLayout (const jam::ConfigDocument& manifest, const Element& sheetRoot, double cellWidth)
{
    const auto rowOffsets { getRowOffsets (getGlyphRowCounts (manifest)) };
    std::array<int, tripleCodepointCount> placedCellCounts {};
    std::array<float, tripleCodepointCount> previousRight {};
    jam::Array<juce::Rectangle<float>> layout;

    for (auto* row : manifest.getTableRows (Id::glyphs))
    {
        const auto glyphText { getGlyphText (manifest, *row) };
        const auto codepointCount { glyphText.length() };
        const auto slot { codepointCount - singleCodepointCount };
        const auto* oldGroup { getCellGroup (sheetRoot, getGroupId (glyphText)) };
        const auto* oldRect { oldGroup != nullptr ? &getCellRect (*oldGroup) : nullptr };
        const auto cellBounds { getCellBounds (codepointCount, placedCellCounts.at (slot), rowOffsets.at (slot),
                                               previousRight.at (slot), getCellWidth (oldRect, codepointCount, cellWidth)) };

        layout.add (cellBounds);
        previousRight.at (slot) = cellBounds.getRight();
        ++placedCellCounts.at (slot);
    }

    return layout;
}

static juce::String getCellsSvg (const jam::ConfigDocument& manifest, const Element& sheetRoot,
                                 const jam::Array<juce::Rectangle<float>>& layout)
{
    const auto cellColour { getColourText (manifest, Id::cell) };
    const auto glyphColour { getColourText (manifest, Id::glyph) };
    juce::String cellsSvg;
    int index { 0 };

    for (auto* row : manifest.getTableRows (Id::glyphs))
    {
        const auto groupId { getGroupId (getGlyphText (manifest, *row)) };

        cellsSvg << getCellSvg (getCellGroup (sheetRoot, groupId), groupId, layout.at (index), cellColour, glyphColour);
        ++index;
    }

    return cellsSvg;
}

static juce::String getGuidesSvg (const jam::ConfigDocument& manifest, const juce::ValueTree& metrics,
                                  const jam::Array<juce::Rectangle<float>>& layout)
{
    const auto labelColour { getColourText (manifest, Id::label) };
    const auto advanceColour { jam::Svg::parseColour (getColourText (manifest, Id::advance)) };
    juce::String guidesSvg;
    int index { 0 };

    for (auto* row : manifest.getTableRows (Id::glyphs))
    {
        guidesSvg << getCellGuidesSvg (manifest, metrics, getGlyphText (manifest, *row), layout.at (index), labelColour, advanceColour);
        ++index;
    }

    return guidesSvg;
}

juce::String getSheetSvg (const jam::ConfigDocument& manifest, const juce::String& sheetText)
{
    const auto sheet { getSheetDocument (sheetText, {}) };
    const auto metrics { manifest.getValueTree (Id::font).getChildWithName (jam::Format::toType (Id::metrics)) };
    const double cellWidth { metrics.getProperty (Id::cellWidth) };
    const auto layout { getCellLayout (manifest, *sheet->getRoot(), cellWidth) };
    auto canvasWidth { static_cast<float> (sheetMarginLeft) };

    for (const auto& cellBounds : layout)
        canvasWidth = juce::jmax (canvasWidth, cellBounds.getRight());

    canvasWidth += static_cast<float> (sheetMarginRight);
    const auto canvasHeight { static_cast<float> (getCanvasHeight (getTotalRowCount (getGlyphRowCounts (manifest)))) };
    const auto* backgroundRect { getBackgroundRect (*sheet->getRoot()) };

    return getDocumentSvg (backgroundRect, canvasWidth, canvasHeight, getGuidesSvg (manifest, metrics, layout),
                           getCellsSvg (manifest, *sheet->getRoot(), layout), getPassthroughSvg (*sheet->getRoot(), backgroundRect),
                           getColourText (manifest, Id::background));
}
