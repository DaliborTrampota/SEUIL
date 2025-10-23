#pragma once

#include <glm/glm.hpp>
#include <variant>

#include "../Configuration.h"
#include "../Events.h"
#include "../Signal.h"


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

        /// @brief Should be called when any mouse event occurs on the root element. Propagates to children. Triggers events.
        /// @param event The mouse event.
        virtual void mouseEvent(const MouseEvent& event);

        /// @brief called by the Renderer when the element is rendered.
        /// @param renderer The renderer.
        virtual void visit(Renderer& renderer) = 0;

        /// @brief Checks if the panel contains a point, eg. mouse position.
        /// @param point The point to check.
        /// @return True if the panel contains the point, false otherwise.
        virtual bool contains(const glm::vec2& point) const;

        EventState state() const { return m_eventState; };

        Signal<const HoverEvent&> hoverSignal;
        Signal<const HoverEvent&> enterSignal;
        Signal<const HoverEvent&> exitSignal;

      protected:
        AnchorPoint m_anchorPoint;
        Positions m_position;
        Sizes m_size;
        glm::ivec4 m_calculatedDims = {0, 0, 0, 0};
        std::shared_ptr<Panel> m_parent = nullptr;

        EventState m_eventState = EventState::None;

        friend class Renderer;
        friend class OpenGLRendererImpl;  // TODO new impls has to be added, bad design?

        /// @brief Calculates the size and position of the element in the hierarchy.
        /// @param parentPosSize The position and size of the parent element.
        /// @return The dimensions of the element. (x = pos.x, y = pos.y, z = size.x, w = size.y)
        glm::ivec4 calculateSizeAndPosition(const glm::ivec4& parentPosSize);

        /// @brief Adjusts the m_calculatedDims based on the anchor point of the element.
        /// @param anchor The anchor point of the element.
        /// @param posSize The position and size of the element.
        /// @param parentPosSize The position and size of the parent element.
        /// @return The position direction modifier for x and y.
        glm::ivec2 adjustAnchorPoint(
            AnchorPoint anchor, glm::ivec4& posSize, const glm::ivec4& parentPosSize
        ) const;
    };
}  // namespace ui