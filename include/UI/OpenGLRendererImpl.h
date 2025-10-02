#pragma once

#include <UI/RendererImpl.h>

#include <glm/glm.hpp>

namespace ui {
    class OpenGLRendererImpl : public RendererImpl {
      public:
        OpenGLRendererImpl(unsigned int fboID, const glm::ivec2& viewportSize);
        ~OpenGLRendererImpl();


      private:
        unsigned int m_fboID = 0;
        unsigned int m_fboTextureID = 0;

        void resize(const glm::ivec2& viewportSize) override;

        void renderQuad(const glm::ivec4& posSize) const override;

        void renderPanel(const Panel& panel) const override;

        void setupFBO();
    };
}  // namespace ui