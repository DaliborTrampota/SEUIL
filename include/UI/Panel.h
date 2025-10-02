#pragma once

#include "Configuration.h"
#include "EventState.h"
#include "UIElement.h"

#include <memory>
#include <variant>
#include <vector>

namespace ui {

    class Panel : public UIElement {
        friend class Renderer;

      public:
        using Positions = std::variant<Pos<Abs, Abs>, Pos<Abs, Rel>, Pos<Rel, Abs>, Pos<Rel, Rel>>;
        using Sizes = std::
            variant<Size<Abs, Abs>, Size<Abs, Rel>, Size<Rel, Abs>, Size<Rel, Rel>, Size<Auto, Auto>>;

        template <typename U, typename V, typename S, typename T>
        Panel(
            Pos<U, V> position,
            Size<S, T> size,
            Style<Panel> style,
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : m_position(position),
              m_size(size),
              m_style(style),
              m_anchorPoint(anchorPoint) {}

        /// @brief Adds a child element to the panel.
        /// @param child The child element to add.
        void addChild(std::shared_ptr<UIElement> child);

        /// @brief Checks if the panel contains a point, eg. mouse position.
        /// @param point The point to check.
        /// @return True if the panel contains the point, false otherwise.
        bool contains(const glm::vec2& point) const override;

        void mouseMove(const glm::ivec2& pos) override;

      protected:
        AnchorPoint m_anchorPoint;
        Positions m_position;
        Sizes m_size;
        Style<Panel> m_style;

        bool m_hoverEvent = false;
        EventState m_eventState = EventState::None;

        std::shared_ptr<Panel> m_parent = nullptr;
        std::vector<std::shared_ptr<UIElement>> m_children;

        glm::ivec2 calculateSizeAndPosition(const glm::ivec2& parentSize) override;
        void visit(Renderer& renderer) override;
    };

}  // namespace ui