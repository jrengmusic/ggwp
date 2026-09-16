#include "Init.h"

using Element = jam::Document::Element;

static constexpr int sheetColumns { 10 };
static constexpr int sheetMarginTop { 70 };
static constexpr int sheetMarginLeft { 40 };
static constexpr int sheetCellWidth { 120 };
static constexpr int sheetCellHeight { 243 };
static constexpr int sheetGutter { 10 };
static constexpr int columnStep { sheetCellWidth + sheetGutter };
static constexpr int rowStep { sheetCellHeight + sheetGutter };

static const juce::String regularSheetPath { "sheets/GlyphSheet-Regular.svg" };

static juce::String getManifestFileName (const juce::String& familyName)
{
    return familyName.toLowerCase().replaceCharacter (' ', '-') + ".md";
}

static juce::String getPostScriptBase (const juce::String& familyName)
{
    return familyName.removeCharacters (" ");
}

static bool isBacktickCell (const juce::String& text)
{
    return text.compare (juce::String::charToString (Chars::backtick)) == 0;
}

static void addHeading (jam::ConfigDocument& document, int level, const juce::String& text)
{
    auto* heading { document.addChild (*document.root, Id::heading) };
    heading->add<int> (Id::type, map::BlockType::heading);
    heading->add<juce::String> (Id::level, juce::String (level));

    auto* headingText { document.addChild (*heading, Id::text) };
    headingText->add<juce::String> (Id::text, text);
}

static void addCell (jam::ConfigDocument& document, Element& row,
                     const juce::Identifier& columnId, const juce::String& text)
{
    auto* cell { document.addChild (row, columnId) };
    cell->add<int> (Id::type, map::BlockType::tableCell);
    cell->add<juce::String> (Id::rawText, text);
}

static Element& addTableRow (jam::ConfigDocument& document, Element& table, const juce::Identifier& rowId)
{
    auto* row { document.addChild (table, rowId) };
    row->add<int> (Id::type, map::BlockType::tableRow);

    return *row;
}

static void addTableBorder (jam::ConfigDocument& document, Element& table)
{
    auto* border { document.addChild (table, Id::border) };
    border->add<int> (Id::type, map::BlockType::tableBorder);
}

static Element& addTable (jam::ConfigDocument& document, const juce::Identifier& tableId)
{
    auto* table { document.addChild (*document.root, tableId) };
    table->add<int> (Id::type, map::BlockType::table);

    return *table;
}

static Element& addKeyTypeValueTable (jam::ConfigDocument& document, const juce::Identifier& tableId)
{
    auto& table { addTable (document, tableId) };
    auto& headerRow { addTableRow (document, table, Id::headerRow) };

    addCell (document, headerRow, Id::key, Id::key.toString());
    addCell (document, headerRow, Id::type, Id::type.toString());
    addCell (document, headerRow, Id::value, Id::value.toString());

    return table;
}

static void addKeyTypeValueRow (jam::ConfigDocument& document, Element& table,
                                const juce::Identifier& key, const juce::Identifier& type,
                                const juce::String& value)
{
    auto& dataRow { addTableRow (document, table, Id::tableRow) };
    addCell (document, dataRow, Id::key, key.toString());
    addCell (document, dataRow, Id::type, type.toString());
    addCell (document, dataRow, Id::value, value);
}

static void addMetricsTable (jam::ConfigDocument& document)
{
    static const juce::String defaultAscender { "881" };
    static const juce::String defaultDescender { "-122" };
    static const juce::String defaultLineGap { "0" };
    static const juce::String defaultCapHeight { "739" };
    static const juce::String defaultXHeight { "552" };
    static const juce::String defaultBaseline { "176" };
    static const juce::String defaultCapline { "31" };
    static const juce::String defaultAdvanceWidthFactor { "5.125" };

    auto& table { addKeyTypeValueTable (document, Id::metrics) };

    addKeyTypeValueRow (document, table, Id::ascender, Id::integer, defaultAscender);
    addKeyTypeValueRow (document, table, Id::descender, Id::integer, defaultDescender);
    addKeyTypeValueRow (document, table, Id::lineGap, Id::integer, defaultLineGap);
    addKeyTypeValueRow (document, table, Id::capHeight, Id::integer, defaultCapHeight);
    addKeyTypeValueRow (document, table, Id::xHeight, Id::integer, defaultXHeight);
    addKeyTypeValueRow (document, table, Id::baseline, Id::integer, defaultBaseline);
    addKeyTypeValueRow (document, table, Id::capline, Id::integer, defaultCapline);
    addKeyTypeValueRow (document, table, Id::cellWidth, Id::integer, juce::String (sheetCellWidth));
    addKeyTypeValueRow (document, table, Id::advanceWidthFactor, Id::floatingPoint, defaultAdvanceWidthFactor);

    addTableBorder (document, table);
}

static void addFontTable (jam::ConfigDocument& document, const juce::String& familyName)
{
    static const juce::String defaultManufacturer { "NONE" };
    static const juce::String defaultVersion { "1.0" };
    static const juce::String copyrightPrefix { "Copyright 2026 " };
    static const juce::String emptyFieldValue { "" };

    auto& table { addKeyTypeValueTable (document, Id::font) };

    addKeyTypeValueRow (document, table, Id::copyright, Id::string, copyrightPrefix + familyName);
    addKeyTypeValueRow (document, table, Id::family, Id::string, familyName);
    addKeyTypeValueRow (document, table, Id::trademark, Id::string, emptyFieldValue);
    addKeyTypeValueRow (document, table, Id::manufacturer, Id::string, defaultManufacturer);
    addKeyTypeValueRow (document, table, Id::designer, Id::string, emptyFieldValue);
    addKeyTypeValueRow (document, table, Id::description, Id::string, emptyFieldValue);
    addKeyTypeValueRow (document, table, Id::url, Id::string, emptyFieldValue);
    addKeyTypeValueRow (document, table, Id::version, Id::string, defaultVersion);

    addTableBorder (document, table);
}

static void addRegularTable (jam::ConfigDocument& document, const juce::String& familyName,
                             const juce::String& postScriptBase)
{
    static const juce::String defaultWeightClass { "400" };

    auto& table { addKeyTypeValueTable (document, Id::regular) };
    const auto regularStyle { map::RibbiStyle::getInstance()->get (map::RibbiStyle::regular) };

    addKeyTypeValueRow (document, table, Id::svg, Id::string, regularSheetPath);
    addKeyTypeValueRow (document, table, Id::style, Id::string, regularStyle);
    addKeyTypeValueRow (document, table, Id::fullName, Id::string, familyName + " " + regularStyle);
    addKeyTypeValueRow (document, table, Id::postScriptName, Id::string, postScriptBase + "-" + regularStyle);
    addKeyTypeValueRow (document, table, Id::weightClass, Id::integer, defaultWeightClass);

    addTableBorder (document, table);
}

static void addGlyphsTable (jam::ConfigDocument& document)
{
    static constexpr int firstGlyphCodepoint { 0x21 };
    static constexpr int lastGlyphCodepoint { 0x7E };

    auto& table { addTable (document, Id::glyphs) };
    auto& headerRow { addTableRow (document, table, Id::headerRow) };

    addCell (document, headerRow, Id::codepoints, Id::codepoints.toString());

    for (int codepoint { firstGlyphCodepoint }; codepoint <= lastGlyphCodepoint; ++codepoint)
    {
        auto& dataRow { addTableRow (document, table, Id::tableRow) };
        const auto cellText { juce::String::charToString (static_cast<juce::juce_wchar> (codepoint)) };

        if (isBacktickCell (cellText))
        {
            addCell (document, dataRow, Id::codepoints, cellText);
        }
        else
        {
            addCell (document, dataRow, Id::codepoints,
                    jam::Format::withEnclosure (cellText, Chars::backtick));
        }
    }

    addTableBorder (document, table);
}

static jam::ConfigDocument getManifestDocument (const juce::String& familyName,
                                                const juce::String& postScriptBase)
{
    static constexpr int titleHeadingLevel { 1 };
    static constexpr int sectionHeadingLevel { 2 };

    jam::ConfigDocument document;

    addHeading (document, titleHeadingLevel, familyName);
    addHeading (document, sectionHeadingLevel, "metrics");
    addMetricsTable (document);
    addHeading (document, sectionHeadingLevel, "font");
    addFontTable (document, familyName);
    addHeading (document, sectionHeadingLevel, "glyphs");
    addGlyphsTable (document);
    addHeading (document, sectionHeadingLevel, "regular");
    addRegularTable (document, familyName, postScriptBase);

    return document;
}

static juce::String getGroupId (const juce::String& text)
{
    jam::Document::Numbers codepoints;

    for (auto cursor { text.getCharPointer() }; not cursor.isEmpty(); ++cursor)
        codepoints.add (static_cast<double> (*cursor));

    return jam::OpenTypeDocument::getGlyphId (codepoints);
}

static juce::String getGlyphGroupSvg (const jam::ConfigDocument& document, Element& row, int rowIndex)
{
    const auto column { rowIndex % sheetColumns };
    const auto rowNumber { rowIndex / sheetColumns };

    const juce::Rectangle<int> cellBounds { sheetMarginLeft + column * columnStep,
                                            sheetMarginTop + rowNumber * rowStep,
                                            sheetCellWidth,
                                            sheetCellHeight };

    auto cellText { document.getTableValue (row, Id::codepoints) };

    if (not isBacktickCell (cellText))
        cellText = jam::Format::withoutEnclosure (cellText, Chars::backtick);

    return jam::Svg::Format::group (jam::Svg::Format::rect (cellBounds), getGroupId (cellText));
}

static juce::String getSheetSvg (const jam::ConfigDocument& document)
{
    static constexpr int sheetBottomMargin { 20 };

    const auto rows { document.getTableRows (Id::glyphs) };
    juce::String cellsSvg;

    for (int rowIndex { 0 }; rowIndex < rows.size(); ++rowIndex)
        cellsSvg << getGlyphGroupSvg (document, *rows.at (rowIndex), rowIndex);

    const auto canvasWidth { sheetMarginLeft + sheetColumns * columnStep - sheetGutter + sheetMarginLeft };
    const auto canvasHeight { sheetMarginTop
                              + ((rows.size() + sheetColumns - 1) / sheetColumns) * rowStep
                              - sheetGutter + sheetBottomMargin };

    return jam::Svg::File::getStringToWrite (canvasWidth, canvasHeight, cellsSvg);
}

static void writeManifestFile (const juce::File& manifestFile, const jam::ConfigDocument& document)
{
    const jam::MarkdownWriter writer;
    const auto manifestText { writer.getText (document) };

    if (not manifestFile.replaceWithText (manifestText))
        juce::ConsoleApplication::fail ("failed to write manifest file: " + manifestFile.getFullPathName());
}

static void writeSheetFile (const juce::File& sheetFile, const jam::ConfigDocument& document)
{
    if (not sheetFile.getParentDirectory().createDirectory().wasOk())
        juce::ConsoleApplication::fail ("failed to create sheets directory: "
                                        + sheetFile.getParentDirectory().getFullPathName());

    const auto sheetText { getSheetSvg (document) };

    if (not sheetFile.replaceWithText (sheetText))
        juce::ConsoleApplication::fail ("failed to write glyph sheet: " + sheetFile.getFullPathName());
}

void runInitCommand (const juce::ArgumentList& args)
{
    args.failIfOptionIsMissing (initOption);

    const auto familyName { args.getValueForOption (initOption) };
    const auto postScriptBase { getPostScriptBase (familyName) };
    const auto document { getManifestDocument (familyName, postScriptBase) };

    const auto manifestFile { juce::File::getCurrentWorkingDirectory().getChildFile (
        getManifestFileName (familyName)) };
    const auto sheetFile { manifestFile.getParentDirectory().getChildFile (regularSheetPath) };

    writeManifestFile (manifestFile, document);
    writeSheetFile (sheetFile, document);

    std::cout << "wrote " << manifestFile.getFullPathName() << " and "
              << sheetFile.getFullPathName() << std::endl;
}
