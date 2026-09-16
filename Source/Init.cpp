#include "Init.h"
#include "Manifest.h"
#include "Sheet.h"

const juce::String initOption { "--init" };

static void writeManifestFile (const juce::File& manifestFile, const jam::ConfigDocument& document)
{
    const jam::MarkdownWriter writer;
    const auto manifestText { writer.getText (document) };

    if (not manifestFile.replaceWithText (manifestText))
        juce::ConsoleApplication::fail ("failed to write manifest file: " + manifestFile.getFullPathName());
}

static void writeSheetFile (const juce::File& sheetFile, const juce::String& sheetText)
{
    if (not sheetFile.getParentDirectory().createDirectory().wasOk())
        juce::ConsoleApplication::fail ("failed to create sheets directory: "
                                        + sheetFile.getParentDirectory().getFullPathName());

    if (not sheetFile.replaceWithText (sheetText))
        juce::ConsoleApplication::fail ("failed to write glyph sheet: " + sheetFile.getFullPathName());
}

static void addMissingGlyphsTable (jam::ConfigDocument& manifest, const juce::File& manifestFile)
{
    if (manifest.getTables (Id::glyphs).isEmpty())
    {
        const auto weightSheetFile { [&] () -> juce::File
        {
            for (auto weight : manifest.getValueTree (Id::font))
                if (weight.getProperty (Id::svg).toString().isNotEmpty())
                {
                    const auto candidateSheetFile { getWeightSheetFile (manifestFile, weight) };

                    if (candidateSheetFile.existsAsFile())
                        return candidateSheetFile;
                }

            return {};
        } () };

        if (weightSheetFile.existsAsFile())
        {
            const auto sheet { getSheetDocument (weightSheetFile.loadFileAsString(), weightSheetFile.getFullPathName()) };

            addGlyphs (manifest, sheet.getRoot());
        }
        else
        {
            addGlyphs (manifest, nullptr);
        }

        writeManifestFile (manifestFile, manifest);
        std::cout << "added glyphs table to " << manifestFile.getFullPathName() << std::endl;
    }
}

static void addMissingGuidesTable (jam::ConfigDocument& manifest, const juce::File& manifestFile)
{
    if (manifest.getTables (Id::guides).isEmpty())
    {
        addGuides (manifest);
        writeManifestFile (manifestFile, manifest);
        std::cout << "added guides table to " << manifestFile.getFullPathName() << std::endl;
    }
}

static void addMissingTables (jam::ConfigDocument& manifest, const juce::File& manifestFile)
{
    addMissingGlyphsTable (manifest, manifestFile);
    addMissingGuidesTable (manifest, manifestFile);
}

static void writeWeightSheets (const jam::ConfigDocument& manifest, const juce::File& manifestFile)
{
    for (auto weight : manifest.getValueTree (Id::font))
    {
        if (weight.getProperty (Id::svg).toString().isNotEmpty())
        {
            const auto weightSheetFile { getWeightSheetFile (manifestFile, weight) };
            const auto sheetText { weightSheetFile.loadFileAsString() };

            writeSheetFile (weightSheetFile, getSheetSvg (manifest, sheetText));
            std::cout << "wrote " << weightSheetFile.getFullPathName() << std::endl;
        }
    }
}

void runInitCommand (const juce::ArgumentList& args)
{
    args.failIfOptionIsMissing (initOption);

    const auto familyName { args.getValueForOption (initOption) };
    const auto postScriptBase { getPostScriptBase (familyName) };

    const auto manifestFile { juce::File::getCurrentWorkingDirectory().getChildFile (
        getManifestFileName (familyName)) };
    const auto sheetFile { manifestFile.getParentDirectory().getChildFile (regularSheetPath) };

    if (not manifestFile.existsAsFile())
    {
        if (sheetFile.existsAsFile())
        {
            const auto sheet { getSheetDocument (sheetFile.loadFileAsString(), sheetFile.getFullPathName()) };
            const auto document { getManifestDocument (familyName, postScriptBase, sheet.getRoot()) };

            writeManifestFile (manifestFile, document);
        }
        else
        {
            const auto document { getManifestDocument (familyName, postScriptBase, nullptr) };

            writeManifestFile (manifestFile, document);
        }

        std::cout << "wrote " << manifestFile.getFullPathName() << std::endl;
    }

    auto builtManifest { jam::ConfigDocument::parse (manifestFile.loadFileAsString(), manifestFile.getFullPathName()) };

    if (const auto result { jam::ConfigValidator::isValid (builtManifest) }; result.failed())
        juce::ConsoleApplication::fail (result.getErrorMessage() + ": " + manifestFile.getFullPathName());

    addMissingTables (builtManifest, manifestFile);

    writeWeightSheets (builtManifest, manifestFile);
}
