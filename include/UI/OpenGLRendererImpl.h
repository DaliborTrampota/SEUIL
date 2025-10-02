#pragma once

#include <UI/RendererImpl.h>

#include <glm/glm.hpp>

namespace ui {
    class OpenGLRendererImpl : public RendererImpl {
      public:
        OpenGLRendererImpl(unsigned int fboID, const glm::ivec2& viewportSize);
        ~OpenGLRendererImpl();

        void resize(const glm::ivec2& viewportSize) override;
        void renderStyledQuad() const override;


        void setupFBO();

      private:
        unsigned int m_fboID = 0;
        unsigned int m_fboTextureID = 0;
    };
}  // namespace ui