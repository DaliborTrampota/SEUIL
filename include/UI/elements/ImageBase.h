#pragma once


#include <filesystem>
#include "../detail/ImageData.h"


namespace fs = std::filesystem;

namespace ui {
    class ImageBase {
      public:
        /// @brief The index of the texture in shader storage buffer object.
        size_t index() const { return m_index; }
        void index(size_t index) { m_index = index; }

        const ImageData* textureData() const { return m_textureData; }

      protected:
        const ImageData* m_textureData = nullptr;
        size_t m_index = std::numeric_limits<size_t>::max();

        void loadTexture(const fs::path& texturePath);
    };
}  // namespace ui