#include <UI/font/FontAtlas.h>
#include <UI/font/FontLoader.h>


#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <msdfgen-ext.h>
#include <msdfgen.h>

using namespace ui;
using namespace msdf_atlas;

namespace {
    /// @returns Charset of unique characters from set B that are not in set A
    Charset unique(const Charset& a, const Charset& b) {
        Charset result;
        for (auto cp : b) {
            if (std::find(a.begin(), a.end(), cp) == a.end())
                result.add(cp);
        }
        return result;
    };
}  // namespace

FontLoader::FontLoader() {
    m_ftHandle = msdfgen::initializeFreetype();
    if (!m_ftHandle)
        throw std::runtime_error("Failed to initialize Freetype");
}

FontLoader::FontLoader(FontLoader&& other) noexcept
    : m_ftHandle(other.m_ftHandle),
      m_loadedFont(other.m_loadedFont),
      m_loadedFontName(std::move(other.m_loadedFontName)),
      m_fontGeometries(std::move(other.m_fontGeometries)),
      m_charsets(std::move(other.m_charsets)),
      m_glyphs(std::move(other.m_glyphs)) {
    // Nullify source pointers to prevent double-free
    other.m_ftHandle = nullptr;
    other.m_loadedFont = nullptr;
}

FontLoader& FontLoader::operator=(FontLoader&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources
        unloadFont();
        if (m_ftHandle) {
            msdfgen::deinitializeFreetype(m_ftHandle);
        }

        // Transfer ownership
        m_ftHandle = other.m_ftHandle;
        m_loadedFont = other.m_loadedFont;
        m_loadedFontName = std::move(other.m_loadedFontName);
        m_fontGeometries = std::move(other.m_fontGeometries);
        m_charsets = std::move(other.m_charsets);
        m_glyphs = std::move(other.m_glyphs);

        // Nullify source pointers
        other.m_ftHandle = nullptr;
        other.m_loadedFont = nullptr;
    }
    return *this;
}

FontLoader::~FontLoader() {
    unloadFont();
    if (m_ftHandle) {
        msdfgen::deinitializeFreetype(m_ftHandle);
    }
}

void FontLoader::loadFont(const std::string& fontPath) {
    if (m_loadedFontName == fontPath)
        return;

    if (m_loadedFont) {
        unloadFont();
        std::cout << "Warning: Unloading font. Unload font first before loading a new one."
                  << std::endl;
    }
    m_loadedFont = msdfgen::loadFont(m_ftHandle, fontPath.c_str());
    m_loadedFontName = fontPath;
    if (!m_loadedFont)
        throw std::runtime_error("Failed to load font: " + fontPath);
}

void FontLoader::unloadFont() {
    if (m_loadedFont) {
        msdfgen::destroyFont(m_loadedFont);
        m_loadedFont = nullptr;
        m_loadedFontName.clear();
    }
}

std::vector<const msdf_atlas::GlyphGeometry*> FontLoader::generateText(
    const std::string& text, unsigned pxSize, int& startIndex, int& count
) {
    if (!m_loadedFont)
        throw std::runtime_error("No font loaded. Load a font first.");

    FontID fontID{m_loadedFontName, pxSize};

    if (m_fontGeometries.find(fontID) == m_fontGeometries.end()) {
        FontGeometry fontGeometry(&m_glyphs);
        m_fontGeometries[fontID] = std::move(fontGeometry);
    }

    FontGeometry& fontGeometry = m_fontGeometries[fontID];
    Charset& charset = m_charsets[fontID];

    Charset newChars;
    std::string charsetString = "\"" + text + "\"";
    newChars.parse(charsetString.c_str(), charsetString.length());

    Charset uniqueChars = unique(charset, newChars);

    startIndex = m_glyphs.size();
    fontGeometry.loadCharset(m_loadedFont, 10.0, uniqueChars);
    count = m_glyphs.size() - startIndex;

    // Preprocess newly added glyphs (edge coloring and wrap box)
    for (size_t i = startIndex; i < m_glyphs.size(); ++i) {
        m_glyphs[i].edgeColoring(&msdfgen::edgeColoringInkTrap, FontLoader::MAX_CORNER_ANGLE, 0);
        m_glyphs[i].wrapBox(
            FontLoader::MINIMUM_SCALE,
            FontLoader::PIXEL_RANGE / FontLoader::MINIMUM_SCALE,
            FontLoader::MITER_LIMIT
        );
    }

    for (auto cp : uniqueChars) {
        charset.add(cp);
    }

    std::vector<const msdf_atlas::GlyphGeometry*> textGlyphs;
    textGlyphs.reserve(text.length());

    std::vector<unicode_t> codepoints;
    codepoints.reserve(text.length());
    utf8Decode(codepoints, text.c_str());

    for (unicode_t cp : codepoints) {
        const GlyphGeometry* glyph = fontGeometry.getGlyph(cp);
        if (glyph) {
            textGlyphs.push_back(glyph);
        }
    }
    return textGlyphs;
}

const msdf_atlas::FontGeometry* FontLoader::getFontGeometry(
    const std::string& fontName, unsigned pxSize
) const {
    FontID fontID{fontName, pxSize};
    auto it = m_fontGeometries.find(fontID);
    if (it == m_fontGeometries.end())
        return nullptr;
    return &it->second;
}

FontAtlas FontLoader::packToAtlas(bool dynamic) {
    if (dynamic) {
        auto atlas = FontAtlas::createDynamic();
        atlas.addGlyphs(m_glyphs.data(), m_glyphs.size());
        return atlas;
    }


    TightAtlasPacker packer;
    packer.setDimensionsConstraint(DimensionsConstraint::SQUARE);
    packer.setMinimumScale(FontLoader::MINIMUM_SCALE);
    packer.setPixelRange(FontLoader::PIXEL_RANGE);
    packer.setMiterLimit(FontLoader::MITER_LIMIT);

    packer.pack(m_glyphs.data(), m_glyphs.size());

    int w, h;
    packer.getDimensions(w, h);
    return FontAtlas::createStatic(w, h, m_glyphs);
}