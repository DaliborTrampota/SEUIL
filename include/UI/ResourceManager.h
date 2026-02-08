#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>


#include "Configuration.h"

namespace ui {
    class FontManager;

    struct TextureHandle {
        size_t id = 0;

        bool isValid() const { return id != 0; }
    };


    class ResourceManager {
      public:
        virtual ~ResourceManager() = default;

        virtual void init(FontManagerType fontManagerType) = 0;
        virtual FontManager& fontManager() = 0;

        virtual TextureHandle loadTexture(const std::string& path, bool pixelated = false) = 0;
        virtual TextureHandle registerNativeTexture(void* nativeHandle) = 0;
        virtual void unloadTexture(TextureHandle handle) = 0;
    };
}  // namespace ui