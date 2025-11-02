#pragma once

#include <UI/RendererImpl.h>

#include <glm/glm.hpp>
#include <vector>

#include "../src/detail/BindlessTextures.h"
#include "../src/detail/Quad.h"
#include "../src/detail/SSBO.h"

#include <UI/font/FontAtlas.h>
#include <UI/font/FontLoader.h>


#include <LWGL/buffer/Attributes.h>
#include <LWGL/render/Material.h>
#include <LWGL/texture/Texture2D.h>

namespace ui {
    class OpenGLRendererImpl : public RendererImpl {
      public:
        OpenGLRendererImpl(unsigned int fboID, const glm::ivec2& viewportSize);
        ~OpenGLRendererImpl();


      private:
        ui::BindlessTextures m_bindlessTextures;
        ui::FontLoader m_fontLoader;
        ui::FontAtlas m_fontAtlas;

        unsigned int m_fboID = 0;
        unsigned int m_fboTextureID = 0;
        unsigned int m_fboRenderbufferID = 0;

        gl::Material m_material;
        gl::Material m_textMaterial;

        gl::Attributes<detail::UIVertex> m_attributes;
        gl::Attributes<detail::UITextVertex> m_textAttributes;

        gl::Texture2D m_textTexture{0};

        detail::SSBO<glm::vec4> m_shaderColors;


        void resize(const glm::ivec2& viewportSize) override;

        detail::Quad makeQuad(const glm::ivec4& posSize) const override;
        detail::TextQuad makeTextQuad(const glm::ivec4& posSize, const glm::vec4& uv) const;

        void render() override;
        void renderPanel(const Panel& panel) override;
        void renderImage(const Image& image) override;
        void renderButton(const Button& button) override;
        void renderLabel(const Label& label) override;

        unsigned int textureID() const override { return m_fboTextureID; }
        void loadImage(Image& image) override;
        void loadText(Label& label) override;
        unsigned int findOrStoreColor(const glm::vec4& color);


        void setupFBO();
        void setupBuffer();

        void beforeRender();
        void afterRender();
    };
}  // namespace ui