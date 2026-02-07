#pragma once

#include "../Events.h"
#include "UIElement.h"

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

        Style<Button>& style() { return m_style; }
        const Style<Button>& style_c() const { return m_style; }

        bool isPressed() const;
        bool isHovered() const;

        Signal<const ButtonEvent&> pressedSignal;
        Signal<const ButtonEvent&> releasedSignal;

      protected:
        Style<Button> m_style;
    };

};  // namespace ui