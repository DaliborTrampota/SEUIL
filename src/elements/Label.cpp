#include <UI/elements/Label.h>

#include <UI/Renderer.h>

using namespace ui;

void Label::visit(Renderer& renderer) {
    renderer.renderLabel(*this);
}

void Label::text(const std::string& text) {
    m_text = text;
    m_textCache.clear();
    m_dirty = true;
    Renderer::markDirty();
}