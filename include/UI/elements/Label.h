#pragma once

#include <string>
#include "UIElement.h"

#include "../ResourceManager.h"

namespace msdf_atlas {
    class GlyphGeometry;
};


namespace ui {
    class Label : public UIElement {
      public:
        template <typename U, typename V>
        Label(
            const std::string& text,
            Pos<U, V> position,
            Style<Label> style = {},
            AnchorPoint anchor = AnchorPoint::None,
            AnchorPoint alignment = AnchorPoint::BottomLeft
        )
            : UIElement(position, Size<Auto, Auto>{}, anchor),
              m_text(text),
              m_style(style),
              m_alignment(alignment) {
            // m_textCache.reserve(text.size());
        }

        Style<Label>& style() { return m_style; }
        const Style<Label>& style_c() const { return m_style; }

        const AnchorPoint& alignment() const { return m_alignment; }

        const std::string& text() const { return m_text; }
        void text(const std::string& text);

        bool contains(const glm::vec2& point) const override { return false; }
        void mouseEvent(const MouseEvent& event) override {};

        const FontHandle& fontHandle() const { return m_fontHandle; }

      protected:
        friend class Renderer;

        Style<Label> m_style;
        AnchorPoint m_alignment;
        std::string m_text;

        bool m_dirty = true;
        FontHandle m_fontHandle;
        TextLayout m_textLayout;
    };
}  // namespace ui