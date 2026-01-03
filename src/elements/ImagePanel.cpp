#include <UI/elements/ImagePanel.h>

#include <UI/Renderer.h>

using namespace ui;

void ImagePanel::visit(Renderer& renderer) {
    renderer.renderImagePanel(*this);
}

// void ImagePanel::setSource(const fs::path& sourcePath) {
//     loadTexture(sourcePath);
//     m_index = std::numeric_limits<size_t>::max();
//     Renderer::markDirty();
// }
