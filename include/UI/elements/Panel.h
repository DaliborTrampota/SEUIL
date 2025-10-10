#pragma once

#include "../Configuration.h"
#include "../EventState.h"
#include "UIElement.h"

#include <memory>
#include <variant>
#include <vector>

namespace ui {

    class Panel : public UIElement {
      public:
        template <typename U, typename V, typename S, typename T>
        Panel(
            Pos<U, V> position,
            Size<S, T> size,
            Style<Panel> style = {},
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : UIElement(position, size, anchorPoint),
              m_style(style) {}

        /// @brief Adds a child element to the panel.
        /// @param child The child element to add.
        void addChild(std::shared_ptr<UIElement> child);
        std::vector<std::shared_ptr<UIElement>>& children() { return m_children; }

        void mouseMove(const glm::ivec2& pos) override;

        // moved it from private to public and removed friend, should it be in public?
        void visit(Renderer& renderer) override;

        Style<Panel>& style() { return m_style; }
        const Style<Panel>& style_c() const { return m_style; }

      protected:
        Style<Panel> m_style;

        bool m_hoverEvent = false;
        EventState m_eventState = EventState::None;

        std::shared_ptr<Panel> m_parent = nullptr;
        std::vector<std::shared_ptr<UIElement>> m_children;
    };

}  // namespace ui