#include "Init.h"
#include "generated/Generated.h"
#include "Manifest.h"
#include "Sheet.h"

const juce::String& getInitOption()
{
    static const juce::String initOption { juce::String (Chars::doubleDash) + Id::init.toString() };
    return initOption;
}

static void addMissingGlyphsTable (jam::ConfigDocument& manifest, const juce::File& manifestFile)
{
    static const juce::Identifier svg { Extensions::svg };

    if (manifest.getTables (Id::glyphs).isEmpty())
    {
        const auto weightSheetFile { [&manifest, &manifestFile] () -> juce::File
        {
            for (auto weight : manifest.getValueTree (Id::font))
                if (weight.getProperty (svg).toString().isNotEmpty())
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
            addGlyphs (manifest, nullptr);
    }
}

static void addMissingTables (jam::ConfigDocument& manifest, const juce::File& manifestFile)
{
    static const std::vector<juce::Identifier> defaultTables { Id::guides, Id::colour };
    const auto tableCount { manifest.getTables().size() };

    addMissingGlyphsTable (manifest, manifestFile);

    for (const auto& tableId : defaultTables)
        if (manifest.getTables (tableId).isEmpty())
            addDefaultTable (manifest, tableId);

    if (manifest.getTables().size() != tableCount)
    {
        writeManifestFile (manifestFile, manifest);
        std::cout << text::Commands::wroteFile << manifestFile.getFullPathName() << std::endl;
    }
}

static void writeWeightSheets (const jam::ConfigDocument& manifest, const juce::File& manifestFile)
{
    static const juce::Identifier svg { Extensions::svg };

    for (auto weight : manifest.getValueTree (Id::font))
    {
        if (weight.getProperty (svg).toString().isNotEmpty())
        {
            const auto weightSheetFile { getWeightSheetFile (manifestFile, weight) };
            const auto sheetText { weightSheetFile.loadFileAsString() };

            writeSheetFile (weightSheetFile, getSheetSvg (manifest, sheetText));
            std::cout << text::Commands::wroteFile << weightSheetFile.getFullPathName() << std::endl;
        }
    }
}

void runInitCommand (const juce::ArgumentList& args)
{
    args.failIfOptionIsMissing (getInitOption());

    const auto familyName { args.getValueForOption (getInitOption()) };
    const auto manifestFile { juce::File::getCurrentWorkingDirectory().getChildFile (
        getManifestFileName (familyName)) };

    auto manifest { manifestFile.existsAsFile() ? getValidatedManifest (manifestFile)
                                                : getDefaultManifest (familyName, getPostScriptBase (familyName)) };

    addMissingTables (manifest, manifestFile);

    writeWeightSheets (manifest, manifestFile);
}
