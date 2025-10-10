#include <UI/Renderer.h>

#include <UI/OpenGLRendererImpl.h>
#include <UI/elements/Panel.h>


#include <glad/glad.h>

using namespace ui;

Renderer::Renderer(RendererType type, const glm::ivec2& viewportSize) {
    switch (type) {
        case RendererType::OpenGL:
            m_impl = std::make_unique<OpenGLRendererImpl>(0, viewportSize);
            break;
    }
}

Renderer::~Renderer() {}

void Renderer::mouseMove(const glm::ivec2& pos) {
    m_root->mouseMove(pos);
}

void Renderer::update(float dt) {
    if (m_dirty) {
        if (m_root) {
            glm::ivec2 viewportSize = m_impl->viewportSize();
            layoutElement(*m_root, {0, 0, viewportSize.x, viewportSize.y});
            render();
        }
        m_dirty = false;
    }
}

void Renderer::setRoot(std::shared_ptr<Panel> root) {
    m_root = root;
    m_dirty = true;
}

void Renderer::setViewportSize(const glm::ivec2& size) {
    m_impl->resize(size);
    m_dirty = true;
}

unsigned int Renderer::textureID() const {
    return m_impl->textureID();
}


void Renderer::layoutElement(UIElement& element, const glm::ivec4& parentDims) {
    const glm::ivec4 calculatedDims = element.calculateSizeAndPosition(parentDims);

    if (auto* panel = dynamic_cast<Panel*>(&element)) {
        for (auto& child : panel->children()) {
            layoutElement(*child, calculatedDims);
        }
    }
}

void Renderer::render() {
    if (m_root) {
        m_root->visit(*this);
        m_impl->render();
    }
}

void Renderer::renderPanel(Panel& panel) {
    m_impl->renderPanel(panel);

    for (auto& child : panel.children()) {
        child->visit(*this);
    }
}