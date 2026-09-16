#include "Manifest.h"

using Element = jam::Document::Element;

const juce::String regularSheetPath { "sheets/GlyphSheet-Regular.svg" };

static constexpr int sectionHeadingLevel { 2 };

static bool isBacktickCell (const juce::String& text)
{
    return text.compare (juce::String::charToString (Chars::backtick)) == 0;
}

juce::String getManifestFileName (const juce::String& familyName)
{
    return familyName.toLowerCase().replaceCharacter (' ', '-') + ".md";
}

juce::String getPostScriptBase (const juce::String& familyName)
{
    return familyName.removeCharacters (" ");
}

static void addHeading (jam::ConfigDocument& document, int level, const juce::String& text)
{
    auto* heading { document.addChild (*document.getRoot(), Id::heading) };
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
    auto* table { document.addChild (*document.getRoot(), tableId) };
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
    auto& dataRow { addTableRow (document, table, key) };
    addCell (document, dataRow, Id::key, key.toString());
    addCell (document, dataRow, Id::type, type.toString());
    addCell (document, dataRow, Id::value, value);
}

static void addGlyphRow (jam::ConfigDocument& document, Element& table, const juce::String& glyphText)
{
    const auto cellText { isBacktickCell (glyphText) ? glyphText
                                                      : jam::Format::withEnclosure (glyphText, Chars::backtick) };
    const auto rowId { juce::Identifier::isValidIdentifier (cellText) ? juce::Identifier (cellText)
                                                                       : Id::tableRow };

    auto& dataRow { addTableRow (document, table, rowId) };

    addCell (document, dataRow, Id::codepoints, cellText);
}

static constexpr int capHeightOffset { 31 };
static constexpr int baselineOffset { 176 };

static void addMetricsTable (jam::ConfigDocument& document)
{
    static constexpr int sheetCellWidth { 120 };
    static const juce::String defaultAscender { "881" };
    static const juce::String defaultDescender { "-122" };
    static const juce::String defaultLineGap { "0" };
    static const juce::String defaultCapHeight { "739" };
    static const juce::String defaultXHeight { "552" };
    static const juce::String defaultAdvanceWidthFactor { "5.125" };

    auto& table { addKeyTypeValueTable (document, Id::metrics) };

    addKeyTypeValueRow (document, table, Id::ascender, Id::integer, defaultAscender);
    addKeyTypeValueRow (document, table, Id::descender, Id::integer, defaultDescender);
    addKeyTypeValueRow (document, table, Id::lineGap, Id::integer, defaultLineGap);
    addKeyTypeValueRow (document, table, Id::capHeight, Id::integer, defaultCapHeight);
    addKeyTypeValueRow (document, table, Id::xHeight, Id::integer, defaultXHeight);
    addKeyTypeValueRow (document, table, Id::baseline, Id::integer, juce::String (baselineOffset));
    addKeyTypeValueRow (document, table, Id::capline, Id::integer, juce::String (capHeightOffset));
    addKeyTypeValueRow (document, table, Id::cellWidth, Id::integer, juce::String (sheetCellWidth));
    addKeyTypeValueRow (document, table, Id::advanceWidthFactor, Id::floatingPoint, defaultAdvanceWidthFactor);

    addTableBorder (document, table);
}

static void addFontTable (jam::ConfigDocument& document, const juce::String& familyName)
{
    static const juce::String defaultManufacturer { "NONE" };
    static const juce::String defaultVersion { "1.0" };
    static const juce::String copyrightPrefix { "Copyright 2026 " };

    auto& table { addKeyTypeValueTable (document, Id::font) };

    addKeyTypeValueRow (document, table, Id::copyright, Id::string, copyrightPrefix + familyName);
    addKeyTypeValueRow (document, table, Id::family, Id::string, familyName);
    addKeyTypeValueRow (document, table, Id::trademark, Id::string, {});
    addKeyTypeValueRow (document, table, Id::manufacturer, Id::string, defaultManufacturer);
    addKeyTypeValueRow (document, table, Id::designer, Id::string, {});
    addKeyTypeValueRow (document, table, Id::description, Id::string, {});
    addKeyTypeValueRow (document, table, Id::url, Id::string, {});
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

static void addGlyphsTable (jam::ConfigDocument& document, const Element* sheetRoot)
{
    static constexpr int firstGlyphCodepoint { 0x21 };
    static constexpr int lastGlyphCodepoint { 0x7E };

    auto& table { addTable (document, Id::glyphs) };
    auto& headerRow { addTableRow (document, table, Id::headerRow) };

    addCell (document, headerRow, Id::codepoints, Id::codepoints.toString());

    if (sheetRoot != nullptr)
    {
        for (auto* group : jam::OpenTypeDocument::getGlyphGroups (*sheetRoot))
        {
            juce::String glyphText;

            for (auto codepoint : jam::OpenTypeDocument::getCodepoints (*group->get<juce::String> (Id::id)))
                glyphText << juce::String::charToString (static_cast<juce::juce_wchar> (codepoint));

            addGlyphRow (document, table, glyphText);
        }
    }
    else
    {
        for (int codepoint { firstGlyphCodepoint }; codepoint <= lastGlyphCodepoint; ++codepoint)
            addGlyphRow (document, table, juce::String::charToString (static_cast<juce::juce_wchar> (codepoint)));
    }

    addTableBorder (document, table);
}

static void addGuideRow (jam::ConfigDocument& document, Element& table,
                         const juce::Identifier& key, int offset, const juce::String& colour)
{
    auto& dataRow { addTableRow (document, table, key) };
    addCell (document, dataRow, Id::key, key.toString());
    addCell (document, dataRow, Id::offset, juce::String (offset));
    addCell (document, dataRow, Id::colour, colour);
}

static void addGuidesTable (jam::ConfigDocument& document)
{
    static constexpr int xHeightGuideOffset { 71 };
    static constexpr int descenderGuideOffset { 217 };
    static const juce::String capHeightGuideColour { "#A78BFA" };
    static const juce::String xHeightGuideColour { "#34D399" };
    static const juce::String baselineGuideColour { "#FACC15" };
    static const juce::String descenderGuideColour { "#F87171" };

    auto& table { addTable (document, Id::guides) };
    auto& headerRow { addTableRow (document, table, Id::headerRow) };

    addCell (document, headerRow, Id::key, Id::key.toString());
    addCell (document, headerRow, Id::offset, Id::offset.toString());
    addCell (document, headerRow, Id::colour, Id::colour.toString());

    addGuideRow (document, table, Id::capHeight, capHeightOffset, capHeightGuideColour);
    addGuideRow (document, table, Id::xHeight, xHeightGuideOffset, xHeightGuideColour);
    addGuideRow (document, table, Id::baseline, baselineOffset, baselineGuideColour);
    addGuideRow (document, table, Id::descender, descenderGuideOffset, descenderGuideColour);

    addTableBorder (document, table);
}

void addGuides (jam::ConfigDocument& document)
{
    addHeading (document, sectionHeadingLevel, Id::guides.toString());
    addGuidesTable (document);
}

void addGlyphs (jam::ConfigDocument& document, const Element* sheetRoot)
{
    addHeading (document, sectionHeadingLevel, Id::glyphs.toString());
    addGlyphsTable (document, sheetRoot);
}

jam::ConfigDocument getManifestDocument (const juce::String& familyName, const juce::String& postScriptBase,
                                         const Element* sheetRoot)
{
    static constexpr int titleHeadingLevel { 1 };

    jam::ConfigDocument document;

    addHeading (document, titleHeadingLevel, familyName);
    addHeading (document, sectionHeadingLevel, Id::metrics.toString());
    addMetricsTable (document);
    addHeading (document, sectionHeadingLevel, Id::font.toString());
    addFontTable (document, familyName);

    addGlyphs (document, sheetRoot);

    addGuides (document);
    addHeading (document, sectionHeadingLevel, Id::regular.toString());
    addRegularTable (document, familyName, postScriptBase);

    return document;
}

juce::String getCellText (const jam::ConfigDocument& document, const Element& row, const juce::Identifier& columnId)
{
    const auto* cell { document.getTableCell (row, columnId) };

    if (cell == nullptr)
    {
        const auto tableId { (row.parent != nullptr ? row.parent->id : row.id).toString() };

        juce::ConsoleApplication::fail ("manifest table " + tableId + " has no " + columnId.toString() + " cell");
    }

    return *cell->get<juce::String> (Id::rawText);
}

juce::File getWeightSheetFile (const juce::File& manifestFile, const juce::ValueTree& weight)
{
    return manifestFile.getParentDirectory().getChildFile (weight.getProperty (Id::svg).toString());
}

juce::String getGlyphText (const jam::ConfigDocument& document, const Element& row)
{
    static constexpr int minimumEnclosedLength { 2 };

    auto cellText { getCellText (document, row, Id::codepoints) };
    const auto isEnclosed { cellText.length() >= minimumEnclosedLength
                            and cellText.startsWithChar (Chars::backtick)
                            and cellText.endsWithChar (Chars::backtick) };

    if (isEnclosed)
        cellText = jam::Format::withoutEnclosure (cellText, Chars::backtick);

    return cellText;
}
