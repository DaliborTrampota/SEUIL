#pragma once

#include "../Configuration.h"
#include "../ResourceManager.h"
#include "UIElement.h"


namespace fs = std::filesystem;

namespace ui {

    class Panel;

    class Image : public UIElement {
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
              m_style(style),
              m_sourcePath(sourcePath) {}

        //void mouseEvent(const MouseEvent& event) override {};

        Style<Image>& style() { return m_style; }
        const Style<Image>& style_c() const { return m_style; }

        void setSource(const fs::path& sourcePath);

        const TextureHandle& resourceHandle() const { return m_textureHandle; }

      protected:
        friend class Renderer;

        Style<Image> m_style;
        fs::path m_sourcePath;

        bool m_dirty = true;
        TextureHandle m_textureHandle;
    };
}  // namespace ui