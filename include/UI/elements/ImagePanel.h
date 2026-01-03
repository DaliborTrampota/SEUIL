#pragma once

#include "../Configuration.h"
#include "ImageBase.h"
#include "Panel.h"

namespace ui {

    class ImagePanel : public Panel,
                       public ImageBase {
      public:
        template <typename U, typename V, typename S, typename T>
        ImagePanel(
            Pos<U, V> position,
            Size<S, T> size,
            Style<ImagePanel> style = {},
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : Panel(position, size, {}, anchorPoint),
              m_style(style) {
            loadTexture(style.backgroundImage);
        }


        // moved it from private to public and removed friend, should it be in public?
        void visit(Renderer& renderer) override;

        Style<ImagePanel>& style() { return m_style; }
        const Style<ImagePanel>& style_c() const { return m_style; }

      protected:
        Style<ImagePanel> m_style;
    };

}  // namespace ui