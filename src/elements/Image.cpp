#include <UI/elements/Image.h>

#include <UI/Renderer.h>


#include <stb_image.h>
#include <format>
#include <stdexcept>


using namespace ui;

ImageData::~ImageData() {
    stbi_image_free(data);
}

ImageData::ImageData(ImageData&& other) noexcept
    : width(other.width),
      height(other.height),
      channels(other.channels),
      data(other.data) {
    other.data = nullptr;  // prevent double free
}

void Image::visit(Renderer& renderer) {
    renderer.renderImage(*this);
}

void Image::loadTexture(const std::string& texturePath) {
    if ((m_textureData = Renderer::imageDataMgr.get(texturePath)))
        return;

    ImageData img;
    stbi_set_flip_vertically_on_load(true);
    img.data = stbi_load(texturePath.c_str(), &img.width, &img.height, &img.channels, 0);

    if (!img.data) {
        throw std::runtime_error(std::format("Failed to load image: {}", stbi_failure_reason()));
    }

    Renderer::imageDataMgr.add(texturePath, std::move(img));
    m_textureData = Renderer::imageDataMgr.get(texturePath);
}