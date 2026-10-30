#include "Extract.h"
#include "generated/Generated.h"
#include "Artwork.h"
#include "Manifest.h"
#include "Sheet.h"

const juce::String& getExtractOption()
{
    static const juce::String extractOption { juce::String (Chars::doubleDash) + Id::extract.toString() };
    return extractOption;
}

static juce::Point<float> getOutlinePoint (const FT_Vector* vector)
{
    return { static_cast<float> (vector->x), static_cast<float> (vector->y) };
}

static const FT_Outline_Funcs& getOutlineFunctions()
{
    static constexpr int outlineShift { 0 };
    static constexpr int outlineDelta { 0 };

    static const FT_Outline_Funcs outlineFunctions
    {
        [] (const FT_Vector* to, void* user)
        {
            auto& path { *static_cast<juce::Path*> (user) };

            path.closeSubPath();
            path.startNewSubPath (getOutlinePoint (to));
            return 0;
        },
        [] (const FT_Vector* to, void* user) { static_cast<juce::Path*> (user)->lineTo (getOutlinePoint (to)); return 0; },
        [] (const FT_Vector* control, const FT_Vector* to, void* user)
        { static_cast<juce::Path*> (user)->quadraticTo (getOutlinePoint (control), getOutlinePoint (to)); return 0; },
        [] (const FT_Vector* firstControl, const FT_Vector* secondControl, const FT_Vector* to, void* user)
        {
            static_cast<juce::Path*> (user)->cubicTo (getOutlinePoint (firstControl), getOutlinePoint (secondControl), getOutlinePoint (to));
            return 0;
        },
        outlineShift,
        outlineDelta
    };

    return outlineFunctions;
}

static juce::Path getGlyphPath (FT_Face face, FT_UInt glyphIndex, const juce::AffineTransform& unitsToSheet)
{
    if (FT_Load_Glyph (face, glyphIndex, FT_LOAD_NO_SCALE) != 0)
        juce::ConsoleApplication::fail (text::Diagnostics::failLoadGlyph + Id::diagnosticSeparator + juce::String (glyphIndex));

    juce::Path path;

    if (FT_Outline_Decompose (&face->glyph->outline, &getOutlineFunctions(), &path) != 0)
        juce::ConsoleApplication::fail (text::Diagnostics::failLoadGlyph + Id::diagnosticSeparator + juce::String (glyphIndex));

    path.closeSubPath();
    path.applyTransform (unitsToSheet);

    return path;
}

static juce::String getGlyphGroupText (const jam::Document::Numbers& codepoints, float cellWidth, const juce::Path& path)
{
    juce::XmlElement group { Id::g.toString() };
    group.setAttribute (Id::id, jam::OpenTypeDocument::getGlyphId (codepoints));

    auto* cell { group.createNewChildElement (Id::rect.toString()) };
    cell->setAttribute (Id::x, 0);
    cell->setAttribute (Id::y, 0);
    cell->setAttribute (Id::width, getCoordinateText (cellWidth));

    group.createNewChildElement (Id::path.toString())->setAttribute (Id::d, jam::Svg::Format::pathToString (path));

    return jam::Xml::getText (*jam::Xml::toDocument (group).getRoot());
}

static jam::HashMap<hb_codepoint_t, juce::String> getLigatureCodepoints (const jam::Typeface::Entry& entry,
                                                                         const jam::Array<hb_codepoint_t>& inputCodepoints)
{
    const std::unique_ptr<hb_buffer_t, decltype (&hb_buffer_destroy)> buffer { hb_buffer_create(), hb_buffer_destroy };
    jam::HashMap<hb_codepoint_t, juce::String> ligatures;

    auto addLigature = [&entry, &buffer, &ligatures] (const juce::String& codepointText)
    {
        hb_buffer_clear_contents (buffer.get());
        hb_buffer_add_utf8 (buffer.get(), codepointText.toRawUTF8(), -1, 0, -1);
        hb_buffer_guess_segment_properties (buffer.get());
        hb_shape (entry.hbFont.get(), buffer.get(), nullptr, 0);

        if (hb_buffer_get_length (buffer.get()) == 1)
            ligatures.insert ({ hb_buffer_get_glyph_infos (buffer.get(), nullptr)->codepoint, codepointText });
    };

    for (auto first : inputCodepoints)
    {
        for (auto second : inputCodepoints)
        {
            const auto pairText { juce::String::charToString (first) + juce::String::charToString (second) };
            addLigature (pairText);

            for (auto third : inputCodepoints)
                addLigature (pairText + juce::String::charToString (third));
        }
    }

    return ligatures;
}

static hb_codepoint_t getUnmappedSubstitution (const jam::Typeface::Entry& entry,
                                               const jam::HashMap<hb_codepoint_t, juce::String>& ligatures)
{
    auto* hbFace { hb_font_get_face (entry.hbFont.get()) };
    const std::unique_ptr<hb_set_t, decltype (&hb_set_destroy)> outputGlyphs { hb_set_create(), hb_set_destroy };
    const auto lookupCount { hb_ot_layout_table_get_lookup_count (hbFace, HB_OT_TAG_GSUB) };
    FT_UInt glyphIndex { 0 };

    for (unsigned int lookupIndex { 0 }; lookupIndex < lookupCount; ++lookupIndex)
        hb_ot_layout_lookup_collect_glyphs (hbFace, HB_OT_TAG_GSUB, lookupIndex, nullptr, nullptr, nullptr, outputGlyphs.get());

    for (auto codepoint { FT_Get_First_Char (entry.face.get(), &glyphIndex) }; glyphIndex != 0;
         codepoint = FT_Get_Next_Char (entry.face.get(), codepoint, &glyphIndex))
        hb_set_del (outputGlyphs.get(), glyphIndex);

    for (const auto& [ligatureGlyph, codepointText] : ligatures)
        hb_set_del (outputGlyphs.get(), ligatureGlyph);

    return hb_set_get_min (outputGlyphs.get());
}

static juce::String getLigatureGroupText (const jam::Typeface::Entry& entry, hb_codepoint_t ligatureGlyph,
                                          const juce::String& codepointText, const juce::AffineTransform& unitsToSheet,
                                          double cellWidth)
{
    jam::Document::Numbers codepoints;

    for (auto codepoint : codepointText)
        codepoints.add (static_cast<double> (codepoint));

    return getGlyphGroupText (codepoints, static_cast<float> (getLigatureCellWidth (codepointText.length(), cellWidth)),
                              getGlyphPath (entry.face.get(), ligatureGlyph, unitsToSheet));
}

static juce::String getLigatures (const jam::Typeface::Entry& entry, const juce::AffineTransform& unitsToSheet, double cellWidth)
{
    auto* hbFace { hb_font_get_face (entry.hbFont.get()) };
    const std::unique_ptr<hb_set_t, decltype (&hb_set_destroy)> inputGlyphs { hb_set_create(), hb_set_destroy };
    const auto lookupCount { hb_ot_layout_table_get_lookup_count (hbFace, HB_OT_TAG_GSUB) };
    jam::Array<hb_codepoint_t> inputCodepoints;
    FT_UInt glyphIndex { 0 };

    for (unsigned int lookupIndex { 0 }; lookupIndex < lookupCount; ++lookupIndex)
        hb_ot_layout_lookup_collect_glyphs (hbFace, HB_OT_TAG_GSUB, lookupIndex, nullptr, inputGlyphs.get(), nullptr, nullptr);

    for (auto codepoint { FT_Get_First_Char (entry.face.get(), &glyphIndex) }; glyphIndex != 0;
         codepoint = FT_Get_Next_Char (entry.face.get(), codepoint, &glyphIndex))
        if (hb_set_has (inputGlyphs.get(), glyphIndex))
            inputCodepoints.add (static_cast<hb_codepoint_t> (codepoint));

    const auto ligatures (getLigatureCodepoints (entry, inputCodepoints));
    const auto unmappedGlyph { getUnmappedSubstitution (entry, ligatures) };
    juce::String ligaturesText;

    if (unmappedGlyph != HB_SET_VALUE_INVALID)
        juce::ConsoleApplication::fail (text::Diagnostics::failUnmappedSubstitution + Id::diagnosticSeparator + juce::String (unmappedGlyph));

    for (const auto& [ligatureGlyph, codepointText] : ligatures)
        ligaturesText << getLigatureGroupText (entry, ligatureGlyph, codepointText, unitsToSheet, cellWidth);

    return ligaturesText;
}

static double getNormalisation (const jam::Typeface::Entry& entry)
{
    return jam::OpenTypeDocument::unitsPerEm / static_cast<double> (entry.upem);
}

static juce::AffineTransform getUnitsToSheet (double normalisation, const juce::ValueTree& metrics)
{
    const double baseline { metrics.getProperty (Id::baseline) };
    const auto glyphToSheet { jam::OpenTypeDocument::getGlyphTransform (jam::OpenTypeDocument::getSheetScale (metrics), baseline, 0.0, 0.0).inverted() };

    return juce::AffineTransform::scale (static_cast<float> (normalisation)).followedBy (glyphToSheet);
}

static float getGlyphCellWidth (FT_Pos horizontalAdvance, double normalisation, int plainAdvance, double cellWidth)
{
    return static_cast<float> (horizontalAdvance * normalisation / plainAdvance * cellWidth);
}

static juce::String getCharacterGroupsText (const jam::Typeface::Entry& entry, const juce::AffineTransform& unitsToSheet,
                                            double normalisation, int plainAdvance, double cellWidth)
{
    const auto face { entry.face.get() };
    juce::String groupsText;
    FT_UInt glyphIndex { 0 };

    for (auto codepoint { FT_Get_First_Char (face, &glyphIndex) }; glyphIndex != 0; codepoint = FT_Get_Next_Char (face, codepoint, &glyphIndex))
    {
        const auto isReservedCodepoint { static_cast<int> (codepoint) <= jam::OpenTypeDocument::spaceCodepoint };

        if (not isReservedCodepoint)
        {
            const auto path { getGlyphPath (face, glyphIndex, unitsToSheet) };
            const auto horizontalAdvance { face->glyph->metrics.horiAdvance };
            const auto isBlankGlyph { path.isEmpty() and horizontalAdvance != 0 };
            jam::Document::Numbers codepoints;

            if (isBlankGlyph)
                juce::ConsoleApplication::fail (text::Diagnostics::failBlankGlyph + Id::diagnosticSeparator
                                                + juce::String::toHexString (static_cast<int> (codepoint)));

            codepoints.add (static_cast<double> (codepoint));
            groupsText << getGlyphGroupText (codepoints, getGlyphCellWidth (horizontalAdvance, normalisation, plainAdvance, cellWidth), path);
        }
    }

    return groupsText;
}

static juce::String getRawSheetText (const jam::Typeface::Entry& entry, const juce::ValueTree& metrics)
{
    const double cellWidth { metrics.getProperty (Id::cellWidth) };
    const auto plainAdvance { jam::OpenTypeDocument::getPlainAdvance (metrics) };
    const auto normalisation { getNormalisation (entry) };
    const auto unitsToSheet { getUnitsToSheet (normalisation, metrics) };

    juce::String rawSheetText;
    rawSheetText << "<svg " << Id::xmlns.toString() << "=\"" << svgNamespace << "\">"
                 << getCharacterGroupsText (entry, unitsToSheet, normalisation, plainAdvance, cellWidth)
                 << getLigatures (entry, unitsToSheet, cellWidth) << "</svg>";

    return rawSheetText;
}

int getFontEntry (jam::Typeface& typeface, const juce::MemoryBlock& fontBytes, const juce::File& fontFile)
{
    const auto juceTypeface { juce::Typeface::createSystemTypefaceFor (fontBytes.getData(), fontBytes.getSize()) };

    if (juceTypeface == nullptr)
        juce::ConsoleApplication::fail (text::Diagnostics::failFontFace + Id::diagnosticSeparator + fontFile.getFullPathName());

    const auto entryIndex { typeface.registerTypeface (juceTypeface, fontBytes.getData(), fontBytes.getSize()) };

    if (typeface.get (entryIndex).face == nullptr)
        juce::ConsoleApplication::fail (text::Diagnostics::failFontFace + Id::diagnosticSeparator + fontFile.getFullPathName());

    return entryIndex;
}

void runExtractCommand (const juce::ArgumentList& args)
{
    static const juce::String manifestOption { juce::String (Chars::doubleDash) + Id::manifest.toString() };
    juce::ScopedJuceInitialiser_GUI juceInitialiser;

    const auto fontFile { args.getExistingFileForOption (getExtractOption()) };
    const auto manifestFile { args.getExistingFileForOption (manifestOption) };

    auto manifest { getValidatedManifest (manifestFile) };
    const auto values { manifest.getValueTree (Id::font) };
    const auto weightId { getValidatedWeight (args, values, manifestFile) };
    juce::MemoryBlock fontBytes;

    if (not fontFile.loadFileAsData (fontBytes))
        juce::ConsoleApplication::fail (text::Diagnostics::failFontFace + Id::diagnosticSeparator + fontFile.getFullPathName());

    jam::Typeface typeface;
    const auto rawSheetText { getRawSheetText (typeface.get (getFontEntry (typeface, fontBytes, fontFile)), values.getChildWithName (Id::toType (Id::metrics))) };

    if (manifest.getTables (Id::glyphs).isEmpty())
    {
        addGlyphs (manifest, getSheetDocument (rawSheetText, fontFile.getFullPathName()).getRoot());
        writeManifestFile (manifestFile, manifest);
    }

    const auto sheetFile { getWeightSheetFile (manifestFile, values.getChildWithName (Id::toType (weightId))) };

    writeSheetFile (sheetFile, getSheetSvg (manifest, rawSheetText));
    std::cout << text::Commands::wroteFile << sheetFile.getFullPathName() << std::endl;
}
