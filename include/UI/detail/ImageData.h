#pragma once

#include <limits>

namespace ui {

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

}  // namespace ui