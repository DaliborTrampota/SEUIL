#include <UI/OpenGLRendererImpl.h>

#include <glad/glad.h>

#include <UI/Panel.h>

#include "../src/detail/Quad.h"
#include "../src/detail/Shader.h"

using namespace ui;

OpenGLRendererImpl::OpenGLRendererImpl(unsigned int fboID, const glm::ivec2& viewportSize)
    : m_fboID(fboID) {
    m_viewportSize = viewportSize;
    setupFBO();
    setupShaders();
    setupBuffer();
}

OpenGLRendererImpl::~OpenGLRendererImpl() {
    if (m_fboTextureID) {
        glDeleteTextures(1, &m_fboTextureID);
    }
    if (m_programID) {
        glDeleteProgram(m_programID);
    }
}

void OpenGLRendererImpl::resize(const glm::ivec2& newSize) {
    m_viewportSize = newSize;
    setupFBO();

    glUseProgram(m_programID);
    glUniform2f(
        glGetUniformLocation(m_programID, "uScreenSize"),
        (float)m_viewportSize.x,
        (float)m_viewportSize.y
    );
    glUseProgram(0);
}

void OpenGLRendererImpl::setupFBO() {
    // Delete old texture and recreate with new size
    if (m_fboTextureID) {
        glDeleteTextures(1, &m_fboTextureID);
    }
    if (m_fboID == 0) {
        glGenFramebuffers(1, &m_fboID);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);

    glGenTextures(1, &m_fboTextureID);
    glBindTexture(GL_TEXTURE_2D, m_fboTextureID);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTextureID, 0);

    glGenRenderbuffers(1, &m_fboRenderbufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fboRenderbufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewportSize.x, m_viewportSize.y);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fboRenderbufferID
    );

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glBindTexture(GL_TEXTURE_2D, 0);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void OpenGLRendererImpl::setupShaders() {
    detail::Shader vertexShader("resources/shaders/SEUIL.vert", GL_VERTEX_SHADER);
    detail::Shader fragmentShader("resources/shaders/SEUIL.frag", GL_FRAGMENT_SHADER);

    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader.ID);
    glAttachShader(m_programID, fragmentShader.ID);
    glLinkProgram(m_programID);

    int success;
    char infoLog[512];
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_programID, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM\n%s", infoLog);
    }

    glUseProgram(m_programID);
    glUniform2f(
        glGetUniformLocation(m_programID, "uScreenSize"), m_viewportSize.x, m_viewportSize.y
    );
    glUseProgram(0);
}

void OpenGLRendererImpl::setupBuffer() {
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    glGenBuffers(1, &m_vboID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    int stride = sizeof(detail::UIVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);  // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(uintptr_t(offsetof(detail::UIVertex, uv)))
    );  // uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(uintptr_t(offsetof(detail::UIVertex, color)))
    );  // color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(uintptr_t(offsetof(detail::UIVertex, roundness)))
    );  // roundness
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(
        4, 1, GL_UNSIGNED_INT, stride, (void*)(uintptr_t(offsetof(detail::UIVertex, type)))
    );  // type
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void OpenGLRendererImpl::beforeRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Clear to fully transparent
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_programID);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_vertices.size() * sizeof(detail::UIVertex),
        m_vertices.data(),
        GL_DYNAMIC_DRAW
    );
}

void OpenGLRendererImpl::afterRender() {
    m_vertices.clear();

    // Restore OpenGL state
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}


detail::Quad OpenGLRendererImpl::makeQuad(
    const glm::ivec4& posSize, const glm::vec4& color, float roundness
) const {
    glm::vec2 BL = {posSize.x, posSize.y + posSize.w};
    glm::vec2 BR = {posSize.x + posSize.z, posSize.y + posSize.w};
    glm::vec2 TL = {posSize.x, posSize.y};
    glm::vec2 TR = {posSize.x + posSize.z, posSize.y};

    detail::Quad quad;
    quad.vertices[0] = {BR, glm::vec2(1.0f, 0.0f), color, roundness};
    quad.vertices[1] = {TL, glm::vec2(0.0f, 1.0f), color, roundness};
    quad.vertices[2] = {BL, glm::vec2(0.0f, 0.0f), color, roundness};
    quad.vertices[3] = {TL, glm::vec2(0.0f, 1.0f), color, roundness};
    quad.vertices[4] = {BR, glm::vec2(1.0f, 0.0f), color, roundness};
    quad.vertices[5] = {TR, glm::vec2(1.0f, 1.0f), color, roundness};

    return quad;
}


void OpenGLRendererImpl::render() {
    beforeRender();

    if (m_vertices.size() > 0) {
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    }

    afterRender();
}

void OpenGLRendererImpl::renderPanel(const Panel& panel) {
    const auto& style = panel.style_c();
    detail::Quad quad = makeQuad(panel.m_calculatedDims, style.backgroundColor, style.roundRadius);

    for (auto& vert : quad.vertices) {
        vert.type = detail::QuadType::Panel;
    }
    m_vertices.insert(m_vertices.end(), std::begin(quad.vertices), std::end(quad.vertices));
}
