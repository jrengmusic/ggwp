#include "Build.h"
#include "generated/Generated.h"
#include "Extract.h"
#include "Manifest.h"

const juce::String& getBuildOption()
{
    static const juce::String buildOption { juce::String (Chars::doubleDash) + Id::build.toString() };
    return buildOption;
}

static juce::String getSheetText (const juce::File& manifestFile, const juce::ValueTree& weight)
{
    const auto sheetFile { getWeightSheetFile (manifestFile, weight) };

    if (not sheetFile.existsAsFile())
        juce::ConsoleApplication::fail (text::Diagnostics::failSheetNotFound + Id::diagnosticSeparator + sheetFile.getFullPathName());

    return sheetFile.loadFileAsString();
}

static void validateGlyphAdvance (FT_Face face, const jam::Document::Element& glyph, int glyphIndex)
{
    const auto actualAdvance { static_cast<int> (face->glyph->metrics.horiAdvance) };
    const auto expectedAdvance { *glyph.get<int> (Id::advance) };

    if (actualAdvance != expectedAdvance)
        juce::ConsoleApplication::fail (text::Diagnostics::failRoundTrip + Id::diagnosticSeparator + text::Diagnostics::failGlyphAdvance
            + Id::diagnosticSeparator + juce::String (glyphIndex) + Id::diagnosticSeparator + juce::String (actualAdvance)
            + Id::diagnosticSeparator + juce::String (expectedAdvance));
}

static void validateGlyphCodepoint (FT_Face face, const jam::Document::Element& glyph, int glyphIndex)
{
    const auto* codepoints { glyph.get<jam::Document::Numbers> (Id::codepoints) };

    if (codepoints->size() == 1)
    {
        const auto codepoint { static_cast<FT_ULong> (codepoints->at (0)) };
        const auto actualGlyphIndex { static_cast<int> (FT_Get_Char_Index (face, codepoint)) };

        if (actualGlyphIndex != glyphIndex)
            juce::ConsoleApplication::fail (text::Diagnostics::failRoundTrip + Id::diagnosticSeparator + text::Diagnostics::failCharIndex
                + Id::diagnosticSeparator + juce::String (codepoint) + Id::diagnosticSeparator + juce::String (actualGlyphIndex)
                + Id::diagnosticSeparator + juce::String (glyphIndex));
    }
}

static void validateGlyphOutline (FT_Face face, const jam::Document::Element& glyph, int glyphIndex)
{
    if (not glyph.get<jam::Document::Numbers> (Id::contourEnds)->isEmpty())
    {
        if (face->glyph->outline.n_contours <= 0)
            juce::ConsoleApplication::fail (text::Diagnostics::failRoundTrip + Id::diagnosticSeparator + text::Diagnostics::failGlyphOutline
                + Id::diagnosticSeparator + juce::String (glyphIndex));
    }
}

static void validateGlyph (FT_Face face, const jam::Document::Element& glyph, int glyphIndex)
{
    if (FT_Load_Glyph (face, static_cast<FT_UInt> (glyphIndex), FT_LOAD_NO_SCALE) != 0)
        juce::ConsoleApplication::fail (text::Diagnostics::failRoundTrip + Id::diagnosticSeparator + text::Diagnostics::failLoadGlyph
            + Id::diagnosticSeparator + juce::String (glyphIndex));

    validateGlyphAdvance (face, glyph, glyphIndex);
    validateGlyphCodepoint (face, glyph, glyphIndex);
    validateGlyphOutline (face, glyph, glyphIndex);
}

static void validateFont (const jam::OpenTypeDocument& document, const jam::Typeface::Entry& entry)
{
    for (auto* glyph : *document.getTable (Id::glyf))
        validateGlyph (entry.face.get(), *glyph, *glyph->get<int> (Id::index));

    const auto actualGlyphCount { static_cast<int> (entry.face->num_glyphs) };
    const auto modelGlyphCount { document.getGlyphCount() };

    if (modelGlyphCount != actualGlyphCount)
        juce::ConsoleApplication::fail (text::Diagnostics::failRoundTrip + Id::diagnosticSeparator + text::Diagnostics::failGlyphCount
            + Id::diagnosticSeparator + juce::String (modelGlyphCount) + Id::diagnosticSeparator + juce::String (actualGlyphCount));
}

void runBuildCommand (const juce::ArgumentList& args)
{
    static const juce::String outOption { juce::String (Chars::doubleDash) + Id::out.toString() };
    juce::ScopedJuceInitialiser_GUI juceInitialiser;

    const auto manifestFile { args.getExistingFileForOption (getBuildOption()) };

    args.failIfOptionIsMissing (outOption);

    const auto manifest { getValidatedManifest (manifestFile) };
    const auto values { manifest.getValueTree (Id::font) };
    const auto weightId { getValidatedWeight (args, values, manifestFile) };
    const auto outputFile { juce::File::getCurrentWorkingDirectory().getChildFile (args.getValueForOption (outOption)) };
    const auto sheetText { getSheetText (manifestFile, values.getChildWithName (Id::toType (weightId))) };

    const jam::OpenTypeDocument document { manifest, weightId, sheetText };
    const jam::OpenTypeWriter writer;
    const auto fontBytes { writer.getBytes (document) };

    if (not outputFile.replaceWithData (fontBytes.getData(), fontBytes.getSize()))
        juce::ConsoleApplication::fail (text::Diagnostics::failFontWrite + Id::diagnosticSeparator + outputFile.getFullPathName());

    jam::Typeface typeface;
    validateFont (document, typeface.get (getFontEntry (typeface, fontBytes, outputFile)));

    std::cout << text::Commands::wroteFile << outputFile.getFullPathName() << " ("
              << document.getGlyphCount() << " glyphs)" << std::endl;
}
