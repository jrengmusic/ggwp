#include "Sheet.h"
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
static constexpr int singleCodepointCount { 1 };
static constexpr int doubleCodepointCount { 2 };
static constexpr int tripleCodepointCount { 3 };
static constexpr int singleGlyphSlot { 0 };
static constexpr int doubleGlyphSlot { 1 };
static constexpr int tripleGlyphSlot { 2 };

static const juce::String groupIndent { jam::Xml::getIndent (1) };
static const juce::String childIndent { jam::Xml::getIndent (2) };

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

static int getManifestCellWidth (const jam::ConfigDocument& manifest)
{
    auto* row { manifest.getTableRow (Id::metrics, Id::cellWidth) };

    if (row != nullptr)
        return getCellText (manifest, *row, Id::value).getIntValue();

    juce::ConsoleApplication::fail ("manifest metrics table has no " + Id::cellWidth.toString() + " row");
}

static int getLigatureCellWidth (int codepointCount, int cellWidth)
{
    return cellWidth * codepointCount + sheetGutter * (codepointCount - 1);
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
    std::array<int, tripleCodepointCount> rowsByFamily { 0, 0, 0 };

    for (auto* row : manifest.getTableRows (Id::glyphs))
    {
        const auto text { getGlyphText (manifest, *row) };
        const auto codepointCount { text.length() };
        const auto isSupportedCodepointCount { codepointCount >= singleCodepointCount and codepointCount <= tripleCodepointCount };

        if (isSupportedCodepointCount)
        {
            const auto slot { codepointCount - singleCodepointCount };
            const auto isReservedCodepoint { codepointCount == singleCodepointCount
                                             and (static_cast<int> (text[0]) == jam::OpenTypeDocument::notdefCodepoint
                                                  or static_cast<int> (text[0]) == jam::OpenTypeDocument::spaceCodepoint) };

            if (isReservedCodepoint)
            {
                juce::ConsoleApplication::fail ("reserved glyph row: " + text);
            }
            else
            {
                ++rowsByFamily.at (slot);
            }
        }
        else
        {
            juce::ConsoleApplication::fail ("unsupported glyph row: " + text);
        }
    }

    return rowsByFamily;
}

static const Element& getCellRect (const Element& group)
{
    auto* cell { group.getChildByID (Id::rect) };

    if (cell == nullptr)
        juce::ConsoleApplication::fail ("glyph group " + group.get<juce::String> (Id::id, {})
                                        + " has no rect cell");

    return *cell;
}

static int getCellWidth (const Element* oldRect, int codepointCount, int cellWidth)
{
    if (oldRect != nullptr)
        return oldRect->get<juce::String> (Id::width, {}).getIntValue();

    return getLigatureCellWidth (codepointCount, cellWidth);
}

static juce::String getCellSvg (const Element* oldGroup, const juce::String& groupId, juce::Rectangle<int> cellBounds)
{
    static const juce::String cellStrokeColour { "#323232" };

    juce::String cellSvg;
    cellSvg << groupIndent << "<g id=\"" << groupId << "\">\n";
    cellSvg << childIndent << "<rect x=\"" << cellBounds.getX() << "\" y=\"" << cellBounds.getY()
            << "\" width=\"" << cellBounds.getWidth() << "\" height=\"" << cellBounds.getHeight()
            << "\" style=\"fill:none;stroke:" << cellStrokeColour << ";stroke-width:" << hairlineStrokeWidth << "\"/>\n";

    if (oldGroup != nullptr)
    {
        const auto& oldRect { getCellRect (*oldGroup) };
        const auto deltaX { cellBounds.getX() - oldRect.get<juce::String> (Id::x, {}).getIntValue() };
        const auto deltaY { cellBounds.getY() - oldRect.get<juce::String> (Id::y, {}).getIntValue() };

        const auto oldGroupId { oldGroup->get<juce::String> (Id::id, {}) };

        for (auto* artwork : *oldGroup)
            if (artwork != &oldRect)
                cellSvg << getArtworkSvg (*artwork, deltaX, deltaY, oldGroupId);
    }

    cellSvg << groupIndent << "</g>\n";
    return cellSvg;
}

static juce::String getDocumentSvg (const Element* backgroundRect, int canvasWidth, int canvasHeight,
                                    const juce::String& guidesSvg, const juce::String& cellsSvg,
                                    const juce::String& passthroughSvg)
{
    static const juce::String svgNamespace { "http://www.w3.org/2000/svg" };
    static const juce::String backgroundColour { "#141414" };

    juce::String sheetSvg;
    sheetSvg << "<svg " << Id::xmlns.toString() << "=\"" << svgNamespace << "\" width=\"" << canvasWidth
             << "\" height=\"" << canvasHeight << "\">\n";

    if (backgroundRect != nullptr)
        sheetSvg << getBackgroundRectSvg (*backgroundRect, canvasWidth, canvasHeight);
    else
        sheetSvg << groupIndent << "<rect width=\"" << canvasWidth << "\" height=\"" << canvasHeight
                 << "\" fill=\"" << backgroundColour << "\"/>\n";

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

jam::XmlDocument getSheetDocument (const juce::String& sheetText, const juce::String& origin)
{
    auto sheet { jam::Xml::parse (sheetText) };

    if (sheetText.isNotEmpty() and sheet.getRoot()->id.isNull())
        juce::ConsoleApplication::fail ("glyph sheet is not valid XML: " + origin);

    return sheet;
}

juce::String getSheetSvg (const jam::ConfigDocument& manifest, const juce::String& sheetText)
{
    const auto sheet { getSheetDocument (sheetText, {}) };

    const auto glyphRowCounts { getGlyphRowCounts (manifest) };
    const auto charRows { getRowCount (glyphRowCounts.at (singleGlyphSlot), sheetColumns) };
    const auto doubleRowCount { getRowCount (glyphRowCounts.at (doubleGlyphSlot),
                                             getLigatureColumns (doubleCodepointCount)) };
    const auto totalRows { charRows + doubleRowCount
                           + getRowCount (glyphRowCounts.at (tripleGlyphSlot),
                                         getLigatureColumns (tripleCodepointCount)) };

    const std::array<int, tripleCodepointCount> columnsPerRow {
        getLigatureColumns (singleCodepointCount), getLigatureColumns (doubleCodepointCount), getLigatureColumns (tripleCodepointCount)
    };
    const std::array<int, tripleCodepointCount> rowOffset { 0, charRows, charRows + doubleRowCount };

    std::array<int, tripleCodepointCount> placedCellCounts { 0, 0, 0 };
    std::array<int, tripleCodepointCount> previousRight { 0, 0, 0 };

    const auto manifestCellWidth { getManifestCellWidth (manifest) };

    juce::String cellsSvg;
    juce::String guidesSvg;
    auto canvasWidth { sheetMarginLeft };

    for (auto* row : manifest.getTableRows (Id::glyphs))
    {
        const auto glyphText { getGlyphText (manifest, *row) };
        const auto codepointCount { glyphText.length() };
        const auto slot { codepointCount - singleCodepointCount };

        const auto column { placedCellCounts.at (slot) % columnsPerRow.at (slot) };
        const auto rowIndex { placedCellCounts.at (slot) / columnsPerRow.at (slot) };
        const auto y { sheetMarginTop + (rowOffset.at (slot) + rowIndex) * rowStep };
        const auto x { column == 0 ? sheetMarginLeft : previousRight.at (slot) + sheetGutter };

        const auto groupId { getGroupId (glyphText) };
        const auto* oldGroup { getCellGroup (*sheet.getRoot(), groupId) };
        const auto* oldRect { oldGroup != nullptr ? &getCellRect (*oldGroup) : nullptr };
        const auto width { getCellWidth (oldRect, codepointCount, manifestCellWidth) };
        const juce::Rectangle<int> cellBounds { x, y, width, sheetCellHeight };

        cellsSvg << getCellSvg (oldGroup, groupId, cellBounds);
        guidesSvg << getCellGuidesSvg (manifest, glyphText, cellBounds);

        canvasWidth = juce::jmax (canvasWidth, cellBounds.getRight());

        previousRight.at (slot) = cellBounds.getRight();
        ++placedCellCounts.at (slot);
    }

    canvasWidth += sheetMarginRight;
    const auto canvasHeight { getCanvasHeight (totalRows) };
    const auto* backgroundRect { getBackgroundRect (*sheet.getRoot()) };
    const auto passthroughSvg { getPassthroughSvg (*sheet.getRoot(), backgroundRect) };

    return getDocumentSvg (backgroundRect, canvasWidth, canvasHeight, guidesSvg, cellsSvg, passthroughSvg);
}
