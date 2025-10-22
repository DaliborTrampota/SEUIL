#pragma once

#include <memory>

#include "../Events.h"
#include "UIElement.h"

struct GLFWcursor;

namespace ui {
    class Button : public UIElement {
      public:
        template <typename U, typename V, typename S, typename T>
        Button(
            Pos<U, V> position,
            Size<S, T> size,
            Style<Button> style = {},
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : UIElement(position, size, anchorPoint),
              m_style(style) {}

        using UIElement::UIElement;

        void mouseEvent(const MouseEvent& event) override;
        void visit(Renderer& renderer) override;

        Style<Button>& style() { return m_style; }
        const Style<Button>& style_c() const { return m_style; }

        bool isPressed() const;
        bool isHovered() const;

        Signal<const ButtonEvent&> pressedSignal;
        Signal<const ButtonEvent&> releasedSignal;

      protected:
        Style<Button> m_style;
        std::shared_ptr<UIElement> m_parent = nullptr;

        inline static GLFWcursor* s_pointerCursor = nullptr;
    };

};  // namespace ui