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
        template <typename U, typename V, typename S, typename T>
        Panel(
            Pos<U, V> position,
            Size<S, T> size,
            Style<Panel> style,
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : UIElement(position, size, anchorPoint),
              m_style(style) {}

        /// @brief Adds a child element to the panel.
        /// @param child The child element to add.
        void addChild(std::shared_ptr<UIElement> child);

        void mouseMove(const glm::ivec2& pos) override;

      protected:
        Style<Panel> m_style;

        bool m_hoverEvent = false;
        EventState m_eventState = EventState::None;

        std::shared_ptr<Panel> m_parent = nullptr;
        std::vector<std::shared_ptr<UIElement>> m_children;

        void visit(Renderer& renderer) override;
    };

}  // namespace ui