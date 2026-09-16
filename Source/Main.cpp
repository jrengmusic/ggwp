#include <ft2build.h>
#include FT_FREETYPE_H
#include "Help.h"
#include "Init.h"
#include "Manifest.h"

static juce::String getSheetText (const juce::File& manifestFile, const juce::ValueTree& weight)
{
    const auto sheetFile { getWeightSheetFile (manifestFile, weight) };

    if (not sheetFile.existsAsFile())
        juce::ConsoleApplication::fail ("glyph sheet not found: " + sheetFile.getFullPathName());

    return sheetFile.loadFileAsString();
}

static const juce::String roundTripFailure { "round trip validation failed: " };
static const juce::String buildOption { "--build" };

static std::unique_ptr<FT_LibraryRec_, decltype (&FT_Done_FreeType)> getFreeTypeLibrary()
{
    FT_Library library { nullptr };

    if (FT_Init_FreeType (&library) != 0)
        juce::ConsoleApplication::fail (roundTripFailure + "FT_Init_FreeType failed");

    return { library, FT_Done_FreeType };
}

static std::unique_ptr<FT_FaceRec_, decltype (&FT_Done_Face)> getValidatedFace (FT_Library library, const juce::MemoryBlock& fontBytes)
{
    static constexpr int firstFaceIndex { 0 };
    FT_Face face { nullptr };

    if (FT_New_Memory_Face (library, static_cast<const FT_Byte*> (fontBytes.getData()),
                            static_cast<FT_Long> (fontBytes.getSize()), firstFaceIndex, &face) != 0)
        juce::ConsoleApplication::fail (roundTripFailure + "FT_New_Memory_Face failed");

    return { face, FT_Done_Face };
}

static void validateGlyphAdvance (FT_Face face, const jam::Document::Element& glyph, int glyphIndex)
{
    const auto actualAdvance { static_cast<int> (face->glyph->metrics.horiAdvance) };
    const auto expectedAdvance { *glyph.get<int> (Id::advance) };

    if (actualAdvance != expectedAdvance)
        juce::ConsoleApplication::fail (roundTripFailure + "glyph " + juce::String (glyphIndex)
            + " FT_Load_Glyph advance " + juce::String (actualAdvance) + ", expected " + juce::String (expectedAdvance));
}

static void validateGlyphCodepoint (FT_Face face, const jam::Document::Element& glyph, int glyphIndex)
{
    const auto* codepoints { glyph.get<jam::Document::Numbers> (Id::codepoints) };

    if (codepoints->size() == 1)
    {
        const auto codepoint { static_cast<FT_ULong> (codepoints->at (0)) };
        const auto actualGlyphIndex { static_cast<int> (FT_Get_Char_Index (face, codepoint)) };

        if (actualGlyphIndex != glyphIndex)
            juce::ConsoleApplication::fail (roundTripFailure + "FT_Get_Char_Index for codepoint "
                + juce::String (codepoint) + " returned glyph " + juce::String (actualGlyphIndex)
                + ", expected " + juce::String (glyphIndex));
    }
}

static void validateGlyphOutline (FT_Face face, const jam::Document::Element& glyph, int glyphIndex)
{
    if (not glyph.get<jam::Document::Numbers> (Id::contourEnds)->isEmpty())
    {
        if (face->glyph->outline.n_contours <= 0)
            juce::ConsoleApplication::fail (roundTripFailure + "glyph " + juce::String (glyphIndex)
                + " has no outline contours");
    }
}

static void validateGlyph (FT_Face face, const jam::Document::Element& glyph, int glyphIndex)
{
    if (FT_Load_Glyph (face, static_cast<FT_UInt> (glyphIndex), FT_LOAD_NO_SCALE) != 0)
        juce::ConsoleApplication::fail (roundTripFailure + "FT_Load_Glyph failed for glyph "
            + juce::String (glyphIndex));

    validateGlyphAdvance (face, glyph, glyphIndex);
    validateGlyphCodepoint (face, glyph, glyphIndex);
    validateGlyphOutline (face, glyph, glyphIndex);
}

static void validateFont (const jam::OpenTypeDocument& document, const juce::MemoryBlock& fontBytes)
{
    const auto library { getFreeTypeLibrary() };
    const auto face { getValidatedFace (library.get(), fontBytes) };

    for (auto* glyph : *document.getTable (Id::glyf))
        validateGlyph (face.get(), *glyph, *glyph->get<int> (Id::index));

    const auto actualGlyphCount { static_cast<int> (face->num_glyphs) };
    const auto modelGlyphCount { document.getGlyphCount() };

    if (modelGlyphCount != actualGlyphCount)
        juce::ConsoleApplication::fail (roundTripFailure + "model " + juce::String (modelGlyphCount)
            + " glyphs, face reports " + juce::String (actualGlyphCount));
}

static void runBuildCommand (const juce::ArgumentList& args)
{
    static const juce::String weightOption { "--weight" };
    static const juce::String outOption { "--out" };

    const auto manifestFile { args.getExistingFileForOption (buildOption) };

    args.failIfOptionIsMissing (weightOption);
    args.failIfOptionIsMissing (outOption);

    const auto weightValue { args.getValueForOption (weightOption) };
    const auto outValue { args.getValueForOption (outOption) };

    const auto manifest { jam::ConfigDocument::parse (manifestFile.loadFileAsString(), manifestFile.getFullPathName()) };
    const auto values { manifest.getValueTree (Id::font) };

    const auto isKnownWeight { [&]
    {
        for (auto weight : values)
            if (weight.getType().toString().compare (weightValue.toUpperCase()) == 0)
                return true;

        return false;
    } () };

    if (not isKnownWeight)
        juce::ConsoleApplication::fail ("unknown weight \"" + weightValue + "\" in manifest: " + manifestFile.getFullPathName());

    const juce::Identifier weightId { weightValue };
    const auto outputFile { juce::File::getCurrentWorkingDirectory().getChildFile (outValue) };
    const auto sheetText { getSheetText (manifestFile, values.getChildWithName (Id::toType (weightId))) };

    const jam::OpenTypeDocument document { manifest, weightId, sheetText };
    const jam::OpenTypeWriter writer;

    const auto fontBytes { writer.getBytes (document) };

    if (not outputFile.replaceWithData (fontBytes.getData(), fontBytes.getSize()))
        juce::ConsoleApplication::fail ("failed to write font file: " + outputFile.getFullPathName());

    validateFont (document, fontBytes);

    std::cout << "wrote " << outputFile.getFullPathName() << " ("
              << document.getGlyphCount() << " glyphs)" << std::endl;
}

int main (int argc, char* argv[])
{
    const map::Generated registries;

    juce::ConsoleApplication app;

    static const juce::String helpOption { "--help|-h" };
    static const juce::String versionOption { "--version|-v" };
    static const juce::String helpFileName { "HELP.md" };
    app.addDefaultCommand ({ helpOption,
                             helpOption,
                             "Prints this guide",
                             "Prints the GGWP guide.",
                             [] (const juce::ArgumentList&) { printHelp (BinaryData::getString (helpFileName)); } });
    app.addVersionCommand (versionOption,
                          juce::String (ProjectInfo::projectName) + " version " + ProjectInfo::versionString);

    app.addCommand ({ buildOption,
                      "--build=<manifest.md> --weight=<weightTableId> --out=<output.ttf>",
                      "Builds a TTF from a manifest and glyph sheet, then validates the round trip",
                      "Parses the manifest, resolves the weight table's glyph sheet relative to the "
                      "manifest's directory, writes the font, and validates it by loading it back "
                      "through FreeType.",
                      runBuildCommand });

    app.addCommand ({ initOption,
                      "--init=<font-name>",
                      "Creates a missing manifest or glyph sheet, or reconciles the two",
                      "Creates whatever is missing: a default manifest, or a matching glyph "
                      "sheet. An existing manifest gains a missing glyphs table, read from its "
                      "glyph sheet. GGWP then reconciles every weight's glyph sheet against the "
                      "manifest's glyph table. It adds and removes cells, and re-lays out the "
                      "canvas. Artwork moves with its own cell, keeping its position relative "
                      "to that cell.",
                      runInitCommand });

    return app.findAndRunCommand (argc, argv);
}
