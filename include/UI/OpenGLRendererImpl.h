#pragma once

#include <UI/RendererImpl.h>

#include <glm/glm.hpp>
#include <vector>

#include "../src/detail/Quad.h"

namespace ui {
    class OpenGLRendererImpl : public RendererImpl {
      public:
        OpenGLRendererImpl(unsigned int fboID, const glm::ivec2& viewportSize);
        ~OpenGLRendererImpl();


      private:
        unsigned int m_fboID = 0;
        unsigned int m_fboTextureID = 0;
        unsigned int m_fboRenderbufferID = 0;

        unsigned int m_programID = 0;
        unsigned int m_vaoID = 0;
        unsigned int m_vboID = 0;
        std::vector<detail::UIVertex> m_vertices;


        void resize(const glm::ivec2& viewportSize) override;

        detail::Quad makeQuad(
            const glm::ivec4& posSize, const glm::vec4& color, unsigned int roundness = 0
        ) const override;

        void render() override;
        void renderPanel(const Panel& panel) override;
        unsigned int textureID() const override { return m_fboTextureID; }


        void setupFBO();
        void setupShaders();
        void setupBuffer();

        void beforeRender();
        void afterRender();
    };
}  // namespace ui