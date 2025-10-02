#include <UI/OpenGLRendererImpl.h>

#include <glad/glad.h>

#include <UI/Panel.h>

using namespace ui;

OpenGLRendererImpl::OpenGLRendererImpl(unsigned int fboID, const glm::ivec2& viewportSize)
    : m_fboID(fboID) {
    m_viewportSize = viewportSize;
    setupFBO();
}

OpenGLRendererImpl::~OpenGLRendererImpl() {
    if (m_fboTextureID) {
        glDeleteTextures(1, &m_fboTextureID);
    }
}

void OpenGLRendererImpl::resize(const glm::ivec2& newSize) {
    m_viewportSize = newSize;
    setupFBO();
}

void OpenGLRendererImpl::setupFBO() {
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


void OpenGLRendererImpl::renderQuad(const glm::ivec4& posSize) const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
    glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRendererImpl::renderPanel(const Panel& panel) const {
    renderQuad(panel.m_calculatedDims);
}
