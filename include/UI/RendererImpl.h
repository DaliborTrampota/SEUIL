#pragma once

#include "detail/Quad.h"

namespace ui {
    class Panel;
    class Image;
    class Button;
    class Label;

    class RendererImpl {
      public:
        friend class Renderer;
        virtual ~RendererImpl() = default;

        virtual void resize(const glm::ivec2& viewportSize) = 0;

        glm::ivec2 viewportSize() const { return m_viewportSize; }

        // template <typename T>
        // virtual void renderStyledQuad(const Style<T>& style) const = 0;
        virtual detail::Quad makeQuad(const glm::ivec4& posSize) const = 0;

        virtual void render() = 0;
        virtual void renderPanel(const Panel& panel) = 0;
        virtual void renderImage(const Image& image) = 0;
        virtual void renderButton(const Button& button) = 0;
        virtual void renderLabel(const Label& label) = 0;

        virtual unsigned int textureID() const = 0;
        virtual void loadImage(Image& image) = 0;
        virtual void loadText(Label& label) = 0;

      protected:
        glm::ivec2 m_viewportSize;
    };
}  // namespace ui