#pragma once

#include <unordered_map>

#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include "FontManager.h"

namespace msdfgen {
    class FreetypeHandle;
    class FontHandle;
    class FontGeometry;
};  // namespace msdfgen

namespace msdf_atlas {
    class FontGeometry;
};  // namespace msdf_atlas

namespace ui {


    struct Font {
        std::string name;
        msdfgen::FontHandle* handle;
        std::unique_ptr<std::vector<msdf_atlas::GlyphGeometry>> glyphs;
        std::unique_ptr<msdf_atlas::FontGeometry> geometry;
    };

    class FTFontManager : public FontManager {
      public:
        static constexpr double PIXEL_RANGE = 2.0;

        FTFontManager();
        ~FTFontManager();

        FontHandle loadFont(const fs::path& path) override final;
        void unloadFont(FontHandle handle) override final;
        TextLayout layoutText(
            const std::string& text,
            FontHandle font,
            float fontSize,
            const glm::vec2& position,
            AnchorPoint alignment
        ) override final;

        glm::ivec2 atlasSize() const override final;
        const unsigned char* atlasData() const override final;
        bool atlasDirty() const override final;
        void markAtlasClean() override final;

      private:
        msdfgen::FreetypeHandle* m_ftHandle;
        std::unordered_map<size_t, Font> m_fonts;

        size_t m_nextFontHandle = 0;

        struct AtlasImpl;
        std::unique_ptr<AtlasImpl> m_atlasImpl;

        static constexpr double MAX_CORNER_ANGLE = 3.0;
        static constexpr double MINIMUM_SCALE = 24.0;
        static constexpr double MITER_LIMIT = 1.0;
    };
}  // namespace ui