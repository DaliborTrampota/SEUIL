#pragma once

#include <string>
#include "UIElement.h"

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
            m_textCache.reserve(text.size());
        }

        Style<Label>& style() { return m_style; }
        const Style<Label>& style_c() const { return m_style; }

        const AnchorPoint& alignment() const { return m_alignment; }

        const std::string& text() const { return m_text; }
        void text(const std::string& text);

        bool contains(const glm::vec2& point) const override { return false; }
        void mouseEvent(const MouseEvent& event) override {};
        void visit(Renderer& renderer) override;

        /// @brief Checks if the text glyphs are generated and cached
        bool cached() const { return !m_dirty && !m_textCache.empty(); }

      protected:
        friend class OpenGLRendererImpl;  //TODO get rid?
        bool m_dirty = true;
        std::string m_text;
        Style<Label> m_style;
        AnchorPoint m_alignment;

        std::vector<const msdf_atlas::GlyphGeometry*> m_textCache;
    };
}  // namespace ui