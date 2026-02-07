#pragma once

#include "UI/ResourceManager.h"
#include "UI/font/FontAtlas.h"
#include "UI/font/FontLoader.h"


#include <LWGL/buffer/SSBO.h>
#include <LWGL/texture/BindlessTextures.h>


namespace ui {
    class LWGLResourceManager : public ResourceManager {
      public:
        LWGLResourceManager();

        void init();

        TextureHandle loadTexture(const std::string& path, bool pixelated = false) override final;
        TextureHandle registerNativeTexture(void* nativeHandle) override final;
        void unloadTexture(TextureHandle handle) override final;

        FontHandle loadFont(const std::string& path) override final;
        void unloadFont(FontHandle handle) override final;
        TextLayout layoutText(
            const std::string& text,
            FontHandle font,
            float fontSize,
            const glm::vec2& position,
            AnchorPoint alignment
        ) override final;

      public:
        unsigned int findOrStoreColor(const glm::vec4& color);
        size_t getBindlessIndex(TextureHandle handle) const;


        gl::BindlessTextures m_bindlessTextures;
        gl::SSBO<glm::vec4> m_colors;
        FontLoader m_fontLoader;
        FontAtlas m_fontAtlas;


      private:
        std::unordered_map<size_t, size_t> m_texHandleToIndex;

        unsigned m_nextTexHandle = 0;
        unsigned m_nextFontHandle = 0;
    };
}  // namespace ui