#pragma once

#include <UI/RendererImpl.h>

#include <glm/glm.hpp>
#include <vector>

#include <UI/detail/Quad.h>

namespace ui {
    class OpenGLRendererImpl : public RendererImpl {
      public:
        OpenGLRendererImpl(unsigned int fboID, const glm::ivec2& viewportSize);
        ~OpenGLRendererImpl();


      private:
        unsigned int m_fboID = 0;
        unsigned int m_fboTextureID = 0;
        unsigned int m_programID = 0;
        std::vector<detail::UIVertex> m_vertices;


        void resize(const glm::ivec2& viewportSize) override;

        detail::Quad makeQuad(
            const glm::ivec4& posSize, const glm::vec4& color, float roundness
        ) const override;

        void render() override;
        void renderPanel(const Panel& panel) const override;


        void setupFBO();
        void setupShaders();
        void setupBuffer();

        void beforeRender();
    };
}  // namespace ui