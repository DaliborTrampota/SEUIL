#include <UI/elements/ImageBase.h>

#include <UI/Renderer.h>
#include <stb_image.h>
#include <format>
#include <stdexcept>

using namespace ui;

void ImageBase::loadTexture(const fs::path& texturePath) {
    if ((m_textureData = Renderer::imageDataMgr.get(texturePath)))
        return;

    ImageData img;
    img.data = stbi_load(texturePath.string().c_str(), &img.width, &img.height, &img.channels, 0);

    if (!img.data) {
        throw std::runtime_error(std::format("Failed to load image: {}", stbi_failure_reason()));
    }

    Renderer::imageDataMgr.add(texturePath, std::move(img));
    m_textureData = Renderer::imageDataMgr.get(texturePath);
}