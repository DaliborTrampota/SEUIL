#pragma once

#include <glm/glm.hpp>

namespace ui {
    class Renderer;

    class UIElement {
      public:
        virtual ~UIElement() = default;

        virtual bool contains(const glm::vec2& point) const = 0;
        virtual void mouseMove(const glm::ivec2& pos) = 0;


      protected:
        friend class Renderer;
        virtual glm::ivec2 calculateSizeAndPosition(const glm::ivec2& parentSize) = 0;
        virtual void visit(Renderer& renderer) = 0;
    };
}  // namespace ui