#include <UI/elements/Image.h>

#include <UI/Renderer.h>

using namespace ui;

void Image::visit(Renderer& renderer) {
    renderer.renderImage(*this);
}

void Image::setSource(const fs::path& sourcePath) {
    loadTexture(sourcePath);
    m_index = std::numeric_limits<size_t>::max();
    Renderer::markDirty();
}
