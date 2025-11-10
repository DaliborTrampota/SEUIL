#include <UI/font/FontAtlas.h>

#include <stdexcept>

#include <msdf-atlas-gen/msdf-atlas-gen.h>

using namespace ui;
using namespace msdf_atlas;


FontAtlas::FontAtlas(AtlasGenerator gen) : m_generator(std::move(gen)) {}
FontAtlas::FontAtlas(DynAtlasGenerator gen) : m_generator(std::move(gen)) {}

FontAtlas::FontAtlas(FontAtlas&& other) noexcept
    : m_width(other.m_width),
      m_height(other.m_height),
      m_generator(std::move(other.m_generator)) {
    // Re-acquire bitmap reference from the moved generator
    if (std::holds_alternative<AtlasGenerator>(m_generator)) {
        m_bitmap = std::get<AtlasGenerator>(m_generator).atlasStorage();
    } else {
        m_bitmap = std::get<DynAtlasGenerator>(m_generator).atlasGenerator().atlasStorage();
    }
}

FontAtlas& FontAtlas::operator=(FontAtlas&& other) noexcept {
    if (this != &other) {
        m_width = other.m_width;
        m_height = other.m_height;
        m_generator = std::move(other.m_generator);

        // Re-acquire bitmap reference from the moved generator
        if (std::holds_alternative<AtlasGenerator>(m_generator)) {
            m_bitmap = std::get<AtlasGenerator>(m_generator).atlasStorage();
        } else {
            m_bitmap = std::get<DynAtlasGenerator>(m_generator).atlasGenerator().atlasStorage();
        }
    }
    return *this;
}

FontAtlas FontAtlas::createStatic(
    int w, int h, const std::vector<msdf_atlas::GlyphGeometry>& glyphs
) {
    FontAtlas atlas(AtlasGenerator(w, h));
    atlas.m_width = w;
    atlas.m_height = h;

    auto& gen = std::get<AtlasGenerator>(atlas.m_generator);
    gen.generate(glyphs.data(), glyphs.size());

    atlas.m_bitmap = std::get<AtlasGenerator>(atlas.m_generator).atlasStorage();

    return atlas;
}

FontAtlas FontAtlas::createDynamic(int threadCount) {
    auto gen = DynAtlasGenerator();
    gen.atlasGenerator().setThreadCount(threadCount);
    return FontAtlas(std::move(gen));
}

void FontAtlas::addGlyphs(msdf_atlas::GlyphGeometry* glyphs, int count) {
    if (std::holds_alternative<AtlasGenerator>(m_generator)) {
        throw std::runtime_error("Cannot add glyphs to static atlas");
    }

    auto& gen = std::get<DynAtlasGenerator>(m_generator);
    DynAtlasGenerator::ChangeFlags change = gen.add(glyphs, count, false);
    m_bitmap = gen.atlasGenerator().atlasStorage();
    //if (change & DynAtlasGenerator::RESIZED) {
    m_width = m_bitmap.width;
    m_height = m_bitmap.height;
    //}
}