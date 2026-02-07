#pragma once

#include "../Configuration.h"
#include "../ResourceManager.h"
#include "UIElement.h"


namespace ui {

    class Panel;

    class Texture : public UIElement {
      public:
        template <typename U, typename V, typename S, typename T>
        Texture(
            Pos<U, V> position,
            Size<S, T> size,
            Style<Texture> style = {},
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : Texture(TextureHandle(), position, size, style, anchorPoint) {}

        template <typename U, typename V, typename S, typename T>
        Texture(
            TextureHandle textureHandle,
            Pos<U, V> position,
            Size<S, T> size,
            Style<Texture> style = {},
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : UIElement(position, size, anchorPoint),
              m_style(style),
              m_textureHandle(textureHandle) {}

        //void mouseEvent(const MouseEvent& event) override {};

        Style<Texture>& style() { return m_style; }
        const Style<Texture>& style_c() const { return m_style; }


        const TextureHandle& resourceHandle() const { return m_textureHandle; }
        void setHandle(TextureHandle textureHandle) {
            m_textureHandle = textureHandle;
            // m_dirty = true;
        }

      protected:
        Style<Texture> m_style;
        TextureHandle m_textureHandle;
    };
}  // namespace ui