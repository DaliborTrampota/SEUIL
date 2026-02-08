#include "UI/font/FTFontManager.h"

#include <msdfgen-ext.h>
#include <msdfgen.h>

#include <msdf-atlas-gen/Charset.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdf-atlas-gen/types.h>

#include "ThreadPoolAtlasGenerator.h"


using namespace ui;

namespace {
    std::string getFontName(const fs::path& path) {
        return path.filename().string();
    }

    void applyAlignment(AnchorPoint alignment, glm::ivec2& pos, const glm::vec2& size) {
        bool top = static_cast<uint8_t>(alignment & AnchorPoint::Top);
        bool bottom = static_cast<uint8_t>(alignment & AnchorPoint::Bottom);
        bool left = static_cast<uint8_t>(alignment & AnchorPoint::Left);
        bool right = static_cast<uint8_t>(alignment & AnchorPoint::Right);
        // no need to check for mid, because it will be handled by the x and y checks

        bool x = left || right;
        bool y = top || bottom;

        if (!x)
            pos.x -= size.x / 2;

        if (!y)
            pos.y += size.y / 2;

        if (top)
            pos.y += size.y;

        if (right)
            pos.x -= size.x;
    }

}  // namespace

struct FTFontManager::AtlasImpl {
    using StorageR = msdfgen::BitmapConstRef<msdf_atlas::byte, 3>;
    using StorageW = msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>;
    using DynAtlasGenerator = msdf_atlas::DynamicAtlas<
        ThreadPoolAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, StorageW>>;

    AtlasImpl(uint8_t threadCount) {
        atlas.atlasGenerator().setThreadCount(threadCount);
        data = atlas.atlasGenerator().atlasStorage();
        size = {data.width, data.height};
    }

    DynAtlasGenerator atlas;
    bool dirty = false;
    glm::ivec2 size = {0, 0};
    StorageR data;
};

FTFontManager::FTFontManager() : m_atlasImpl(std::make_unique<AtlasImpl>(4)) {
    m_ftHandle = msdfgen::initializeFreetype();
    if (!m_ftHandle)
        throw std::runtime_error("Failed to initialize Freetype");
}

FTFontManager::~FTFontManager() {
    msdfgen::deinitializeFreetype(m_ftHandle);
}

FontHandle FTFontManager::loadFont(const fs::path& path) {
    std::string fontName = getFontName(path);
    for (const auto& [handle, font] : m_fonts) {
        if (font.name == fontName) {
            return FontHandle{handle};
        }
    }

    msdfgen::FontHandle* loadedFont = msdfgen::loadFont(m_ftHandle, path.string().c_str());
    if (!loadedFont)
        throw std::runtime_error("Failed to load font: " + path.string());

    Font font = {
        .name = fontName,
        .handle = loadedFont,
        .glyphs = std::make_unique<std::vector<msdf_atlas::GlyphGeometry>>(),
        .geometry = {}
    };
    font.geometry = std::make_unique<msdf_atlas::FontGeometry>(font.glyphs.get());

    font.geometry->loadCharset(loadedFont, 1.0, msdf_atlas::Charset::ASCII);
    for (msdf_atlas::GlyphGeometry& glyph : *font.glyphs) {
        glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, MAX_CORNER_ANGLE, 0);
        glyph.wrapBox(MINIMUM_SCALE, PIXEL_RANGE / MINIMUM_SCALE, MITER_LIMIT);
    }

    AtlasImpl::DynAtlasGenerator::ChangeFlags change =
        m_atlasImpl->atlas.add(font.glyphs->data(), font.glyphs->size(), false);
     m_atlasImpl->data = m_atlasImpl->atlas.atlasGenerator().atlasStorage();
    m_atlasImpl->size = {m_atlasImpl->data.width, m_atlasImpl->data.height};
    m_atlasImpl->dirty = true;

    FontHandle handle{m_nextFontHandle++};
    m_fonts[handle.id] = std::move(font);
    return handle;
}


void FTFontManager::unloadFont(FontHandle handle) {
    auto it = m_fonts.find(handle.id);
    if (it != m_fonts.end()) {
        msdfgen::destroyFont(it->second.handle);
        m_fonts.erase(it->first);
    }
}

TextLayout FTFontManager::layoutText(
    const std::string& text,
    FontHandle fontHandle,
    float fontSize,
    const glm::vec2& position,
    AnchorPoint alignment
) {
    Font& font = m_fonts[fontHandle.id];

    std::vector<msdf_atlas::unicode_t> codepoints;
    codepoints.reserve(text.length());
    msdf_atlas::utf8Decode(codepoints, text.c_str());

    std::vector<const msdf_atlas::GlyphGeometry*> msdfGlyphs;
    msdfGlyphs.reserve(codepoints.size());
    for (const auto& codepoint : codepoints) {
        const msdf_atlas::GlyphGeometry* glyph = font.geometry->getGlyph(codepoint);
        if (glyph) {
            msdfGlyphs.push_back(glyph);
        } else {
            throw std::runtime_error(
                "Failed to get glyph for codepoint: " + std::to_string(codepoint)
            );
        }
    }

    TextLayout layout;
    layout.glyphs.reserve(text.length());

    float advance = 0.f;
    float maxHeight = 0.f;
    float scale = fontSize / font.geometry->getMetrics().emSize;

    for (const auto& glyph : msdfGlyphs) {
        if (!glyph->isWhitespace()) {
            double l, b, r, t;
            glyph->getQuadPlaneBounds(l, b, r, t);

            glm::vec4 posSize = {
                l * scale,
                -t * scale,  // invert Y: use -t for top
                (r - l) * scale,
                (t - b) * scale
            };
            posSize.x += advance;

            maxHeight = std::max(maxHeight, posSize.w);

            glm::dvec4 uv;
            glyph->getQuadAtlasBounds(uv.x, uv.y, uv.z, uv.w);
            // TODO revisit the swap
            std::swap(uv.y, uv.w);  // Flip UV vertically (swap top and bottom)

            layout.glyphs.push_back(
                {.rect = posSize,
                 .atlasPixelCoords = glm::vec4(uv.x, uv.y, uv.z, uv.w),
                 .advance = advance}
            );
        }

        advance += glyph->getAdvance() * scale;
    }

    layout.position = position;
    layout.dimensions = {advance, maxHeight};

    applyAlignment(alignment, layout.position, layout.dimensions);

    return layout;
}


glm::ivec2 FTFontManager::atlasSize() const {
    return m_atlasImpl->size;
}

bool FTFontManager::atlasDirty() const {
    return m_atlasImpl->dirty;
}

void FTFontManager::markAtlasClean() {
    m_atlasImpl->dirty = false;
}

const unsigned char* FTFontManager::atlasData() const {
    return m_atlasImpl->data.pixels;
}