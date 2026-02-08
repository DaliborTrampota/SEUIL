#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <string>
#include <vector>


#include "../Configuration.h"

namespace fs = std::filesystem;

namespace ui {

    struct FontHandle {
        size_t id = 0;

        bool isValid() const { return id != 0; }
    };


    struct GlyphInstance {
        glm::vec4 rect;
        glm::vec4 atlasPixelCoords;
        float advance;
    };

    struct TextLayout {
        std::vector<GlyphInstance> glyphs;
        glm::ivec2 position;
        glm::vec2 dimensions;
    };
    class FontManager {
      public:
        virtual ~FontManager() = default;

        virtual FontHandle loadFont(const fs::path& path) = 0;
        virtual void unloadFont(FontHandle handle) = 0;
        virtual TextLayout layoutText(
            const std::string& text,
            FontHandle font,
            float fontSize,
            const glm::vec2& position,
            AnchorPoint alignment
        ) = 0;

        virtual glm::ivec2 atlasSize() const = 0;
        virtual const unsigned char* atlasData() const = 0;
        virtual bool atlasDirty() const = 0;
        virtual void markAtlasClean() = 0;
    };
}  // namespace ui