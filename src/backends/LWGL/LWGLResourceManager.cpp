#include "LWGLResourceManager.h"

#include <LWGL/texture/ImageData.h>
#include <LWGL/texture/Texture2D.h>
#include <LWGL/texture/TextureRef.h>

using namespace ui;

LWGLResourceManager::LWGLResourceManager() : m_fontAtlas(FontAtlas::createDynamic()) {}

void LWGLResourceManager::init() {
    m_colors.create({});
}

unsigned int LWGLResourceManager::findOrStoreColor(const glm::vec4& color) {
    const std::vector<glm::vec4>& data = m_colors.data();
    auto it = std::find(data.begin(), data.end(), color);
    if (it == data.end()) {
        m_colors.add(color);
        return data.size() - 1;
    }
    return std::distance(data.begin(), it);
}

TextureHandle LWGLResourceManager::loadTexture(const std::string& path, bool pixelated) {
    gl::ImageData imageData(path.c_str());

    gl::TextureParams params =
        pixelated ? gl::TextureParams::Pixelated()
                  : gl::TextureParams(gl::TextureParams::MirroredRepeat, gl::TextureParams::Linear);

    gl::Texture2D texture = gl::Texture2D::fromImageData(imageData, params);
    size_t index = m_bindlessTextures.add(std::move(texture));

    TextureHandle handle{m_nextTexHandle++};
    m_texHandleToIndex[handle.id] = index;
    return handle;
}

TextureHandle LWGLResourceManager::registerNativeTexture(void* nativeHandle) {
    gl::TextureRef textureRef = *static_cast<gl::TextureRef*>(nativeHandle);

    size_t index = m_bindlessTextures.add(textureRef);

    TextureHandle handle{m_nextTexHandle++};
    m_texHandleToIndex[handle.id] = index;
    return handle;
}

void LWGLResourceManager::unloadTexture(TextureHandle handle) {
    if (auto it = m_texHandleToIndex.find(handle.id); it != m_texHandleToIndex.end()) {
        size_t index = it->second;
        m_bindlessTextures.destroy(index);
        m_texHandleToIndex.erase(it);
    }
}

size_t LWGLResourceManager::getBindlessIndex(TextureHandle handle) const {
    if (auto it = m_texHandleToIndex.find(handle.id); it != m_texHandleToIndex.end()) {
        return it->second;
    }
    return std::numeric_limits<size_t>::max();
}

FontHandle LWGLResourceManager::loadFont(const std::string& path) {
    return {};
}

void LWGLResourceManager::unloadFont(FontHandle handle) {}

TextLayout LWGLResourceManager::layoutText(
    const std::string& text,
    FontHandle font,
    float fontSize,
    const glm::vec2& position,
    AnchorPoint alignment
) {
    return {};
}