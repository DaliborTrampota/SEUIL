#include "LWGLRendererImpl.h"

#include <variant>
#include "Vertex.h"

using namespace ui;

namespace {
    template <typename T>
    T makeQuad(const glm::ivec4& posSize) {
        glm::vec2 TL = {posSize.x, posSize.y};
        glm::vec2 TR = {posSize.x + posSize.z, posSize.y};
        glm::vec2 BL = {posSize.x, posSize.y + posSize.w};
        glm::vec2 BR = {posSize.x + posSize.z, posSize.y + posSize.w};

        T quad;
        // The vert shader's NDC has Y inverted
        quad.vertices[0] = {BR, glm::vec2(1.0f, 1.0f)};
        quad.vertices[1] = {TL, glm::vec2(0.0f, 0.0f)};
        quad.vertices[2] = {BL, glm::vec2(0.0f, 1.0f)};
        quad.vertices[3] = {TL, glm::vec2(0.0f, 0.0f)};
        quad.vertices[4] = {BR, glm::vec2(1.0f, 1.0f)};
        quad.vertices[5] = {TR, glm::vec2(1.0f, 0.0f)};

        return quad;
    }

}  // namespace


LWGLRendererImpl::LWGLRendererImpl(const glm::ivec2& viewportSize)
    : m_outputTexture(false),
      m_quadMaterial(
          "resources/shaders/SEUIL.vert", "resources/shaders/SEUIL_QUAD.frag", "SEUIL_QUAD", true
      ),
      m_imageMaterial(
          "resources/shaders/SEUIL.vert", "resources/shaders/SEUIL_IMAGE.frag", "SEUIL_IMAGE", true
      ),
      m_textMaterial(
          "resources/shaders/SEUIL.vert", "resources/shaders/SEUIL_TEXT.frag", "SEUIL_TEXT", true
      ) {
    m_quadMaterial.setConstant("vertex_layout", "{{QuadLayout.glsl}}");
    m_imageMaterial.setConstant("vertex_layout", "{{ImageLayout.glsl}}");
    m_textMaterial.setConstant("vertex_layout", "{{TextLayout.glsl}}");

    m_quadMaterial.compile();
    m_imageMaterial.compile();
    m_textMaterial.compile();

    m_viewportSize = viewportSize;
    m_resourceManager.init();
    m_quadAttributes.create();
    m_imageAttributes.create();
    m_textAttributes.create();


    m_outputTexture.create(
        gl::TextureParams(gl::TextureParams::ClampToEdge, gl::TextureParams::Linear)
    );
    m_outputTexture.allocate(
        {.width = m_viewportSize.x, .height = m_viewportSize.y, .format = gl::ImageFormat::RGBA}
    );

    m_fbo.bind();
    m_fbo.bindTexture(gl::FBOAttachment::Color, &m_outputTexture);

    m_rbo.create(m_viewportSize.x, m_viewportSize.y, GL_DEPTH24_STENCIL8, 0);
    m_fbo.attachRenderBuffer(m_rbo, gl::FBOAttachment::DepthStencil);
    assert(m_fbo.checkCompleteness() == 0);
    m_fbo.clearActive({0.0f, 0.0f, 0.0f, 0.0f}, 1.0f, 0);
    m_fbo.unbind();

    // TODO UBO
    m_quadMaterial.use();
    m_quadMaterial.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
    m_imageMaterial.use();
    m_imageMaterial.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
    m_textMaterial.use();
    m_textMaterial.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
}

void LWGLRendererImpl::resize(const glm::ivec2& newSize) {
    if (m_viewportSize == newSize)
        return;

    m_viewportSize = newSize;

    m_outputTexture.bind();
    m_outputTexture.allocate(
        {.width = newSize.x, .height = newSize.y, .format = gl::ImageFormat::RGBA}
    );
    m_rbo.create(newSize.x, newSize.y, GL_DEPTH24_STENCIL8, 0);

    m_fbo.bind();
    m_fbo.attachRenderBuffer(m_rbo, gl::FBOAttachment::DepthStencil);
    assert(m_fbo.checkCompleteness() == 0);
    m_fbo.unbind();

    m_quadMaterial.use();
    m_quadMaterial.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
    m_imageMaterial.use();
    m_imageMaterial.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
    m_textMaterial.use();
    m_textMaterial.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
}

void LWGLRendererImpl::submitCommand(const DrawCommand& command) {
    m_commands.push_back(command);
}

void LWGLRendererImpl::flush() {
    for (const auto& command : m_commands) {
        std::visit([this](const auto& cmd) { draw(cmd); }, command);
    }
    m_commands.clear();

    m_fbo.bind();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Clear to fully transparent
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_resourceManager.m_colors.bind(1);

    if (m_quadAttributes.length() > 0) {
        m_quadMaterial.use();
        m_quadAttributes.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_quadAttributes.length());
        m_quadAttributes.clear();
    }
    if (m_imageAttributes.length() > 0) {
        m_resourceManager.m_bindlessTextures.use();
        m_resourceManager.m_bindlessTextures.update();

        m_imageMaterial.use();
        m_imageAttributes.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_imageAttributes.length());
        m_imageAttributes.clear();

        // Unload because we dont render UI every frame
        m_resourceManager.m_bindlessTextures.unload();
    }

    if (m_textAttributes.length() > 0) {
        m_textMaterial.use();
        m_textMaterial.setFloat("pxRange", FontLoader::PIXEL_RANGE);
        m_textMaterial.bindTextures();
        m_textAttributes.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_textAttributes.length());
        m_textAttributes.clear();
    }

    m_resourceManager.m_colors.unbind(1);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}


void LWGLRendererImpl::draw(const DrawQuad& quad) {
    lwgl::Quad<lwgl::QuadVertex> vertQuad = makeQuad<lwgl::Quad<lwgl::QuadVertex>>(quad.rect);

    unsigned int colorIndex = m_resourceManager.findOrStoreColor(quad.color);
    unsigned int borderColorIndex = m_resourceManager.findOrStoreColor({quad.borderColor, 1.f});

    for (auto& vert : vertQuad.vertices) {
        vert.colorIndex = colorIndex;
        vert.borderData = {quad.roundRadius, quad.borderThickness, borderColorIndex};
        // vert.type = detail::QuadType::Panel;
        m_quadAttributes.move(std::move(vert));
    }
}

void LWGLRendererImpl::draw(const DrawImage& image) {
    lwgl::Quad<lwgl::ImageVertex> vertQuad = makeQuad<lwgl::Quad<lwgl::ImageVertex>>(image.rect);

    size_t imageIdx = m_resourceManager.getBindlessIndex(image.texture);

    for (auto& vert : vertQuad.vertices) {
        vert.imageIndex = imageIdx;
        vert.opacity = image.opacity;
        vert.roundRadius = image.roundRadius;
        m_imageAttributes.move(std::move(vert));
    }
}

void LWGLRendererImpl::draw(const DrawText& text) {}
