#pragma once

#include <glm/glm.hpp>
#include <string>


#include "Configuration.h"

namespace ui {

    struct FontHandle {
        size_t id = 0;

        bool isValid() const { return id != 0; }
    };

    struct TextureHandle {
        size_t id = 0;

        bool isValid() const { return id != 0; }
    };

    // std::vector<const msdf_atlas::GlyphGeometry*> m_textCache;
    struct TextLayout {};

    class ResourceManager {
      public:
        virtual ~ResourceManager() = default;

        virtual TextureHandle loadTexture(const std::string& path, bool pixelated = false) = 0;
        virtual TextureHandle registerNativeTexture(void* nativeHandle) = 0;
        virtual void unloadTexture(TextureHandle handle) = 0;

        virtual FontHandle loadFont(const std::string& path) = 0;
        virtual void unloadFont(FontHandle handle) = 0;
        virtual TextLayout layoutText(
            const std::string& text,
            FontHandle font,
            float fontSize,
            const glm::vec2& position,
            AnchorPoint alignment
        ) = 0;
    };
}  // namespace ui