#pragma once

#include <UI/elements/UIElement.h>
#include "../Configuration.h"
#include "../EventState.h"


#include <memory>
#include <string>

namespace ui {

    class Panel;

    struct ImageData {
        int width;
        int height;
        int channels;
        unsigned char* data;

        ImageData() = default;
        // TODO move ctor
        ImageData(ImageData&& other) noexcept;
        ~ImageData();
    };

    class Image : public UIElement {
      public:
        template <typename U, typename V, typename S, typename T>
        Image(
            const char* texturePath,
            Pos<U, V> position,
            Size<S, T> size,
            Style<Image> style = {},
            AnchorPoint anchorPoint = AnchorPoint::None
        )
            : UIElement(position, size, anchorPoint),
              m_style(style) {
            loadTexture(texturePath);
        }

        void mouseMove(const glm::ivec2& pos) override;
        void visit(Renderer& renderer) override;

        Style<Image>& style() { return m_style; }
        const Style<Image>& style_c() const { return m_style; }

        /// @brief The index of the texture in shader storage buffer object.
        size_t index() const { return m_index; }
        void index(size_t index) { m_index = index; }

        const ImageData* textureData() const { return m_textureData; }

      protected:
        Style<Image> m_style;

        bool m_hoverEvent = false;
        EventState m_eventState = EventState::None;

        std::shared_ptr<Panel> m_parent = nullptr;

        const ImageData* m_textureData = nullptr;
        size_t m_index = std::numeric_limits<size_t>::max();

      private:
        void loadTexture(const std::string& texturePath);
    };
}  // namespace ui