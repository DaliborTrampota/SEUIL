#pragma once

#include "../Configuration.h"
#include "../ResourceManager.h"
#include "Panel.h"

namespace ui {

    class ImagePanel : public Panel {
      public:
        template <typename U, typename V, typename S, typename T>
        ImagePanel(
            Pos<U, V> position,
            Size<S, T> size,
            Style<ImagePanel> style = {},
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : Panel(position, size, {}, anchorPoint),
              m_style(style) {}


        Style<ImagePanel>& style() { return m_style; }
        const Style<ImagePanel>& style_c() const { return m_style; }

        const TextureHandle& resourceHandle() const { return m_textureHandle; }

      protected:
        friend class Renderer;

        Style<ImagePanel> m_style;

        bool m_dirty = true;
        TextureHandle m_textureHandle;
    };

}  // namespace ui