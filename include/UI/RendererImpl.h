#pragma once

#include <glm/glm.hpp>
#include "Configuration.h"

#include "detail/Quad.h"

namespace ui {
    class Panel;

    class RendererImpl {
      public:
        friend class Renderer;
        virtual ~RendererImpl() = default;

        virtual void resize(const glm::ivec2& viewportSize) = 0;

        glm::ivec2 viewportSize() const { return m_viewportSize; }

        // template <typename T>
        // virtual void renderStyledQuad(const Style<T>& style) const = 0;
        virtual detail::Quad makeQuad(
            const glm::ivec4& posSize, const glm::vec4& color, float roundness
        ) const = 0;

        virtual void render() = 0;
        virtual void renderPanel(const Panel& panel) const = 0;

      protected:
        glm::ivec2 m_viewportSize;
    };
}  // namespace ui