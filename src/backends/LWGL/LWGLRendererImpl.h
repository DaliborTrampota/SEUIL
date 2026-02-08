#pragma once

#include <UI/render/DrawCommands.h>
#include <UI/render/RendererImpl.h>
#include <glm/glm.hpp>


#include <LWGL/buffer/Attributes.h>
#include <LWGL/buffer/FBO.h>
#include <LWGL/buffer/RBO.h>
#include <LWGL/render/ShaderProgram.h>
#include <LWGL/texture/Texture2D.h>


#include "LWGLResourceManager.h"
#include "Vertex.h"


namespace ui {
    class Panel;
    class ImagePanel;
    class Image;
    class Button;
    class Label;
    class Texture;


    class LWGLRendererImpl : public RendererImpl {
      public:
        friend class Renderer;
        LWGLRendererImpl(const glm::ivec2& viewportSize, FontManagerType fontManagerType);
        ~LWGLRendererImpl() = default;

        void resize(const glm::ivec2& viewportSize) override final;
        void submitCommand(const DrawCommand& command) override final;
        void flush() override final;

        ResourceManager& resourceManager() override final { return m_resourceManager; }

        NativeOutputHandle outputHandle() const override final {
            return NativeOutputHandle{
                RendererType::LWGL, static_cast<const void*>(&m_outputTexture)
            };
        }

        void updateAtlas();

      public:
        void draw(const DrawQuad& quad);
        void draw(const DrawImage& image);
        void draw(const DrawText& text);

      private:
        gl::FBO m_fbo;
        gl::RBO m_rbo;
        gl::Texture2D m_outputTexture;
        gl::Texture2D m_atlasTexture{false};

        std::vector<DrawCommand> m_commands;
        LWGLResourceManager m_resourceManager;

        gl::Attributes<ui::lwgl::QuadVertex> m_quadAttributes;
        gl::Attributes<ui::lwgl::ImageVertex> m_imageAttributes;
        gl::Attributes<ui::lwgl::TextVertex> m_textAttributes;

        gl::ShaderProgram m_quadMaterial;
        gl::ShaderProgram m_imageMaterial;
        gl::ShaderProgram m_textShaderMSDF;
    };
}  // namespace ui