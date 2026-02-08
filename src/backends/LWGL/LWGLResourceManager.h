#pragma once

#include "UI/ResourceManager.h"

#include <LWGL/buffer/SSBO.h>
#include <LWGL/texture/BindlessTextures.h>


namespace ui {
    class LWGLResourceManager : public ResourceManager {
      public:
        LWGLResourceManager();

        void init(FontManagerType fontManagerType) override final;
        FontManager& fontManager() override final { return *m_fontManager; }

        TextureHandle loadTexture(const std::string& path, bool pixelated = false) override final;
        TextureHandle registerNativeTexture(void* nativeHandle) override final;
        void unloadTexture(TextureHandle handle) override final;


      public:
        unsigned int findOrStoreColor(const glm::vec4& color);
        size_t getBindlessIndex(TextureHandle handle) const;


        gl::BindlessTextures m_bindlessTextures;
        gl::SSBO<glm::vec4> m_colors;


      private:
        std::unordered_map<size_t, size_t> m_texHandleToIndex;
        std::unique_ptr<FontManager> m_fontManager;

        unsigned m_nextTexHandle = 0;
        unsigned m_nextFontHandle = 0;
    };
}  // namespace ui