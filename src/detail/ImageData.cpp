#include <UI/detail/ImageData.h>

#include <stb_image.h>

using namespace ui;

ImageData::~ImageData() {
    stbi_image_free(data);
}

ImageData::ImageData(ImageData&& other) noexcept
    : width(other.width),
      height(other.height),
      channels(other.channels),
      data(other.data) {
    other.data = nullptr;
}