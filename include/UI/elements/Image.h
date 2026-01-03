#pragma once

#include "../Configuration.h"
#include "ImageBase.h"
#include "UIElement.h"


namespace ui {

    class Panel;

    class Image : public UIElement,
                  public ImageBase {
      public:
        template <typename U, typename V, typename S, typename T>
        Image(
            const fs::path& sourcePath,
            Pos<U, V> position,
            Size<S, T> size,
            Style<Image> style = {},
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : UIElement(position, size, anchorPoint),
              m_style(style) {
            loadTexture(sourcePath);
        }

        //void mouseEvent(const MouseEvent& event) override {};
        void visit(Renderer& renderer) override;

        Style<Image>& style() { return m_style; }
        const Style<Image>& style_c() const { return m_style; }

        void setSource(const fs::path& sourcePath);

      protected:
        Style<Image> m_style;
    };
}  // namespace ui