#pragma once

#include <UI/RendererImpl.h>

#include <glm/glm.hpp>

#include "../src/detail/BindlessTextures.h"
#include "../src/detail/SSBO.h"

#include <UI/detail/Quad.h>
#include <UI/font/FontAtlas.h>
#include <UI/font/FontLoader.h>


#include <LWGL/buffer/Attributes.h>
#include <LWGL/buffer/FBO.h>
#include <LWGL/buffer/RBO.h>
#include <LWGL/render/ShaderProgram.h>
#include <LWGL/texture/Texture2D.h>


namespace ui {
    class OpenGLRendererImpl : public RendererImpl {
      public:
        OpenGLRendererImpl(const glm::ivec2& viewportSize);


      private:
        ui::BindlessTextures m_bindlessTextures;
        ui::FontLoader m_fontLoader;
        ui::FontAtlas m_fontAtlas;

        gl::FBO m_fbo;
        gl::RBO m_rbo;
        gl::Texture2D m_outputTexture;

        gl::ShaderProgram m_material;
        gl::ShaderProgram m_textMaterial;

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

        unsigned int textureID() const override { return m_outputTexture.id(); }
        void loadImage(Image& image) override;
        void loadText(Label& label) override;
        unsigned int findOrStoreColor(const glm::vec4& color);

        void setupFBO();

        void beforeRender();
        void afterRender();
    };
}  // namespace ui