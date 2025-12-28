#pragma once

#include "../Configuration.h"
#include "UIElement.h"


#include <filesystem>

namespace fs = std::filesystem;

namespace ui {

    class Panel;

    struct ImageData {
        int width;
        int height;
        int channels;
        unsigned char* data;

        size_t index = std::numeric_limits<size_t>::max();

        ImageData() = default;
        // TODO move ctor
        ImageData(ImageData&& other) noexcept;
        ~ImageData();
    };

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
              m_style(style) {
            loadTexture(sourcePath);
        }

        //void mouseEvent(const MouseEvent& event) override {};
        void visit(Renderer& renderer) override;

        Style<Image>& style() { return m_style; }
        const Style<Image>& style_c() const { return m_style; }

        /// @brief The index of the texture in shader storage buffer object.
        size_t index() const { return m_index; }
        void index(size_t index) { m_index = index; }

        const ImageData* textureData() const { return m_textureData; }

        void setSource(const fs::path& sourcePath);

      protected:
        Style<Image> m_style;

        const ImageData* m_textureData = nullptr;
        size_t m_index = std::numeric_limits<size_t>::max();

      private:
        void loadTexture(const fs::path& texturePath);
    };
}  // namespace ui