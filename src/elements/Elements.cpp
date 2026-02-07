#include <UI/Renderer.h>
#include <UI/elements/Image.h>
// #include <UI/elements/ImagePanel.h>
#include <UI/elements/Label.h>


using namespace ui;


void Image::setSource(const fs::path& sourcePath) {
    m_sourcePath = sourcePath;
    m_dirty = true;
    Renderer::markDirty();
}

void Label::text(const std::string& text) {
    m_text = text;
    m_dirty = true;
    Renderer::markDirty();
}

// TODO change imagePanel background?