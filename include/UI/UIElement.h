#pragma once

#include <glm/glm.hpp>
#include <variant>

#include "Configuration.h"

namespace ui {
    class Renderer;

    class UIElement {
      public:
        using Positions = std::variant<Pos<Abs, Abs>, Pos<Abs, Rel>, Pos<Rel, Abs>, Pos<Rel, Rel>>;
        using Sizes = std::
            variant<Size<Abs, Abs>, Size<Abs, Rel>, Size<Rel, Abs>, Size<Rel, Rel>, Size<Auto, Auto>>;

        template <typename U, typename V, typename S, typename T>
        UIElement(Pos<U, V> position, Size<S, T> size, AnchorPoint anchorPoint = AnchorPoint::None)
            : m_position(position),
              m_size(size),
              m_anchorPoint(anchorPoint) {}

        virtual ~UIElement() = default;

        virtual void mouseMove(const glm::ivec2& pos) = 0;

        /// @brief Checks if the panel contains a point, eg. mouse position.
        /// @param point The point to check.
        /// @return True if the panel contains the point, false otherwise.
        bool contains(const glm::vec2& point) const;


      protected:
        AnchorPoint m_anchorPoint;
        Positions m_position;
        Sizes m_size;
        glm::ivec4 m_calculatedDims = {0, 0, 0, 0};

        friend class Renderer;
        /// @brief Calculates the size and position of the element in the hierarchy.
        /// @param parentPosSize The position and size of the parent element.
        /// @return The dimensions of the element. (x = pos.x, y = pos.y, z = size.x, w = size.y)
        glm::ivec4 calculateSizeAndPosition(const glm::ivec4& parentPosSize);
        virtual void visit(Renderer& renderer) = 0;
    };
}  // namespace ui