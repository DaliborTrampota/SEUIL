#include <UI/Renderer.h>

#include <UI/Panel.h>

#include <glad/glad.h>

using namespace ui;

Renderer::Renderer(unsigned int fboID, const glm::ivec2& viewportSize)
    : m_fboID(fboID),
      m_viewportSize(viewportSize) {
    setupFBO();
}

Renderer::~Renderer() {
    if (m_fboTextureID) {
        glDeleteTextures(1, &m_fboTextureID);
    }
}

void Renderer::setupFBO() {
    // Delete old texture and recreate with new size
    if (m_fboTextureID) {
        glDeleteTextures(1, &m_fboTextureID);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);

    glGenTextures(1, &m_fboTextureID);
    glBindTexture(GL_TEXTURE_2D, m_fboTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        m_viewportSize.x,
        m_viewportSize.y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTextureID, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::mouseMove(const glm::ivec2& pos) {
    m_root->mouseMove(pos);
}

void Renderer::update(float dt) {
    if (m_dirty) {
        if (m_root) {
            layoutElement(*m_root, m_viewportSize);
        }
        m_dirty = false;
    }
}

void Renderer::setRoot(std::shared_ptr<Panel> root) {
    m_root = root;
    m_dirty = true;
}

void Renderer::setViewportSize(const glm::ivec2& size) {
    m_viewportSize = size;
    resize();
}

void Renderer::resize() {
    m_dirty = true;
}

void Renderer::layoutElement(UIElement& element, const glm::ivec2& parentSize) {
    const glm::ivec2 calculatedSize = element.calculateSizeAndPosition(parentSize);

    if (auto* panel = dynamic_cast<Panel*>(&element)) {
        for (auto& child : panel->m_children) {
            layoutElement(*child, calculatedSize);
        }
    }
}

void Renderer::render() {
    if (m_root) {
        m_root->visit(*this);
    }
}

void Renderer::renderPanel(Panel& panel) {
    //render to fbo
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
    glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    for (auto& child : panel.m_children) {
        child->visit(*this);
    }
}