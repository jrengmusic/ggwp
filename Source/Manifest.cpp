#include "Manifest.h"
#include "generated/Generated.h"

using Element = jam::Document::Element;

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
    auto* heading { document.addChild (*document.getRoot(), map::HeadingTag::getInstance()->get (level)) };
    heading->add<int> (Id::type, map::BlockType::heading);
    heading->add<int> (Id::level, level);
    heading->add<juce::String> (Id::id, text);
    heading->add<jam::Document::Identifiers> (Id::classes);

    auto* headingText { document.addChild (*heading, Id::text) };
    headingText->add<juce::String> (Id::text, text);
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

static const jam::Strings& getGlyphColumns()
{
    static const jam::Strings columns { Id::codepoints.toString() };
    return columns;
}

static void addGlyphRow (jam::ConfigDocument& document, Element& table, const juce::String& glyphText)
{
    const auto cellText { isBacktickCell (glyphText) ? glyphText
                                                      : jam::Format::withEnclosure (glyphText, Chars::backtick) };
    const jam::Strings texts { cellText };

    document.addRow (table, cellText, getGlyphColumns(), texts);
}

static void addGlyphsTable (jam::ConfigDocument& document, const Element* sheetRoot)
{
    static constexpr int firstGlyphCodepoint { 0x21 };
    static constexpr int lastGlyphCodepoint { 0x7E };

    auto& table { addTable (document, Id::glyphs) };

    document.addRow (table, Id::headerRow.toString(), getGlyphColumns(), getGlyphColumns());

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

void addGlyphs (jam::ConfigDocument& document, const Element* sheetRoot)
{
    addHeading (document, map::HeadingLevel::level2, Id::glyphs.toString());
    addGlyphsTable (document, sheetRoot);
}

jam::ConfigDocument getDefaultManifest (const juce::String& familyName, const juce::String& postScriptBase)
{
    const auto manifestText { jam::Format::replaceholder (
        jam::Format::replaceholder (BinaryData::getString (files::ggwpManifest), "family", familyName, "@"),
        "postScriptBase", postScriptBase, "@") };

    return jam::ConfigDocument::parse (manifestText, files::ggwpManifest);
}

void addDefaultTable (jam::ConfigDocument& manifest, const juce::Identifier& tableId)
{
    const auto defaults { getDefaultManifest ({}, {}) };

    addHeading (manifest, map::HeadingLevel::level2, tableId.toString());

    for (const auto* defaultTable : defaults.getTables (tableId))
    {
        auto& table { addTable (manifest, tableId) };
        const auto headers (defaults.getTableHeaders (*defaultTable));
        jam::Strings columns;

        for (const auto& header : headers)
            columns.add (header.toString());

        manifest.addRow (table, Id::headerRow.toString(), columns, columns);

        for (const auto* defaultRow : defaults.getTableRows (*defaultTable))
        {
            jam::Strings texts;

            for (const auto& header : headers)
                texts.add (getCellText (defaults, *defaultRow, header));

            manifest.addRow (table, defaultRow->id.toString(), columns, texts);
        }

        addTableBorder (manifest, table);
    }
}

juce::String getCellText (const jam::ConfigDocument& document, const Element& row, const juce::Identifier& columnId)
{
    const auto* cell { document.getTableCell (row, columnId) };

    if (cell == nullptr)
    {
        const auto tableId { (row.parent != nullptr ? row.parent->id : row.id).toString() };

        juce::ConsoleApplication::fail (text::Diagnostics::failManifestCell + Id::diagnosticSeparator + tableId
                                        + Id::diagnosticSeparator + columnId.toString());
    }

    return jam::MarkdownDocument::getRawText (*cell, document.getSource());
}

juce::String getColourText (const jam::ConfigDocument& manifest, const juce::Identifier& key)
{
    const auto* row { manifest.getTableRow (Id::colour, key) };

    if (row != nullptr)
        return getCellText (manifest, *row, Id::colour);

    juce::ConsoleApplication::fail (text::Diagnostics::failManifestCell + Id::diagnosticSeparator + Id::colour.toString()
                                    + Id::diagnosticSeparator + key.toString());
}

jam::ConfigDocument getValidatedManifest (const juce::File& manifestFile)
{
    auto manifest { jam::ConfigDocument::parse (manifestFile.loadFileAsString(), manifestFile.getFullPathName()) };

    if (const auto result { jam::ConfigValidator {}.isValid (manifest) }; result.failed())
        juce::ConsoleApplication::fail (result.getErrorMessage() + Id::diagnosticSeparator + manifestFile.getFullPathName());

    return manifest;
}

void writeManifestFile (const juce::File& manifestFile, const jam::ConfigDocument& document)
{
    const jam::MarkdownWriter writer;
    const auto manifestText { writer.getText (document) };

    if (not manifestFile.replaceWithText (manifestText))
        juce::ConsoleApplication::fail (text::Diagnostics::failManifestWrite + Id::diagnosticSeparator + manifestFile.getFullPathName());
}

juce::Identifier getValidatedWeight (const juce::ArgumentList& args, const juce::ValueTree& values, const juce::File& manifestFile)
{
    static const juce::String weightOption { juce::String (Chars::doubleDash) + Id::weight.toString() };

    args.failIfOptionIsMissing (weightOption);

    const auto weightValue { args.getValueForOption (weightOption) };

    if (weightValue.isNotEmpty())
    {
        const juce::Identifier weightId { weightValue };

        if (values.getChildWithName (Id::toType (weightId)).isValid())
            return weightId;
    }

    juce::ConsoleApplication::fail (text::Diagnostics::failWeightUnknown + Id::diagnosticSeparator + weightValue
        + Id::diagnosticSeparator + manifestFile.getFullPathName());
}

juce::File getWeightSheetFile (const juce::File& manifestFile, const juce::ValueTree& weight)
{
    static const juce::Identifier svg { Extensions::svg };

    return manifestFile.getParentDirectory().getChildFile (weight.getProperty (svg).toString());
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
