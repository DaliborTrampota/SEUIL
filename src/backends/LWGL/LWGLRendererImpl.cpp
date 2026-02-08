#include "LWGLRendererImpl.h"

#include <variant>
#include "Vertex.h"

#include "UI/font/FTFontManager.h"

using namespace ui;

namespace {

    template <typename T>
    T makeQuad(const glm::ivec4& posSize, glm::vec4 uv = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)) {
        glm::vec2 TL = {posSize.x, posSize.y};
        glm::vec2 TR = {posSize.x + posSize.z, posSize.y};
        glm::vec2 BL = {posSize.x, posSize.y + posSize.w};
        glm::vec2 BR = {posSize.x + posSize.z, posSize.y + posSize.w};

        T quad;
        // The vert shader's NDC has Y inverted
        quad.vertices[0] = {BR, glm::vec2(uv.z, uv.w)};
        quad.vertices[1] = {TL, glm::vec2(uv.x, uv.y)};
        quad.vertices[2] = {BL, glm::vec2(uv.x, uv.w)};
        quad.vertices[3] = {TL, glm::vec2(uv.x, uv.y)};
        quad.vertices[4] = {BR, glm::vec2(uv.z, uv.w)};
        quad.vertices[5] = {TR, glm::vec2(uv.z, uv.y)};

        return quad;
    }
}  // namespace

LWGLRendererImpl::LWGLRendererImpl(const glm::ivec2& viewportSize, FontManagerType fontManagerType)
    : m_outputTexture(false),
      m_quadMaterial(
          "resources/shaders/SEUIL.vert", "resources/shaders/SEUIL_QUAD.frag", "SEUIL_QUAD", true
      ),
      m_imageMaterial(
          "resources/shaders/SEUIL.vert", "resources/shaders/SEUIL_IMAGE.frag", "SEUIL_IMAGE", true
      ),
      m_textShaderMSDF(
          "resources/shaders/SEUIL.vert", "resources/shaders/SEUIL_TEXT.frag", "SEUIL_Text", true
      ) {
    m_quadMaterial.setConstant("vertex_layout", "{{QuadLayout.glsl}}");
    m_imageMaterial.setConstant("vertex_layout", "{{ImageLayout.glsl}}");
    m_textShaderMSDF.setConstant("vertex_layout", "{{TextLayout.glsl}}");

    m_quadMaterial.compile();
    m_imageMaterial.compile();
    m_textShaderMSDF.compile();


    m_viewportSize = viewportSize;
    m_resourceManager.init(fontManagerType);
    m_quadAttributes.create();
    m_imageAttributes.create();
    m_textAttributes.create();

    // TODO use active shader based on the font manager type
    if (auto mgr = dynamic_cast<FTFontManager*>(&m_resourceManager.fontManager())) {
        m_textShaderMSDF.use();
        m_textShaderMSDF.setFloat("pxRange", FTFontManager::PIXEL_RANGE);
        m_textShaderMSDF.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
    }

    m_outputTexture.create(
        gl::TextureParams(gl::TextureParams::ClampToEdge, gl::TextureParams::Linear)
    );
    m_outputTexture.allocate(
        {.width = m_viewportSize.x, .height = m_viewportSize.y, .format = gl::ImageFormat::RGBA}
    );
    m_atlasTexture.create(
        gl::TextureParams(gl::TextureParams::ClampToEdge, gl::TextureParams::Linear)
    );

    m_textShaderMSDF.use();
    m_textShaderMSDF.setTexture(TEXTURE_UNIT_MSDF, &m_atlasTexture, "uMSDF");

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
    // TODO set to active shader
    m_textShaderMSDF.use();
    m_textShaderMSDF.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
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
    // if (auto mgr = dynamic_cast<FTFontManager*>(m_fontManager)) {
    // TODO use active shader here
    if (m_textAttributes.length() > 0) {
        m_textShaderMSDF.use();
        m_textShaderMSDF.bindTextures();
        m_textAttributes.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_textAttributes.length());
        m_textAttributes.clear();
    }
    // }

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

void LWGLRendererImpl::draw(const DrawText& text) {
    if (m_resourceManager.fontManager().atlasDirty()) {
        updateAtlas();
    }
    glm::ivec2 atlasSize = m_resourceManager.fontManager().atlasSize();
    for (const GlyphInstance& glyph : text.layout.glyphs) {
        glm::vec4 absoluteRect = glyph.rect;
        absoluteRect.x += text.layout.position.x;
        absoluteRect.y += text.layout.position.y;

        glm::vec4 uv = glyph.atlasPixelCoords;
        uv.x /= atlasSize.x;
        uv.y /= atlasSize.y;
        uv.z /= atlasSize.x;
        uv.w /= atlasSize.y;

        lwgl::Quad<lwgl::TextVertex> vertQuad =
            makeQuad<lwgl::Quad<lwgl::TextVertex>>(absoluteRect, uv);

        unsigned int colorIndex = m_resourceManager.findOrStoreColor(text.color);
        for (auto& vert : vertQuad.vertices) {
            vert.colorIndex = colorIndex;
            m_textAttributes.move(std::move(vert));
        }
    }
}

void LWGLRendererImpl::updateAtlas() {
    glm::ivec2 atlasSize = m_resourceManager.fontManager().atlasSize();
    m_atlasTexture.allocate({
        .width = atlasSize.x,
        .height = atlasSize.y,
        .format = gl::ImageFormat::RGB,
        .dataType = gl::ImageDataType::UChar,
    });
    m_atlasTexture.upload(gl::ImageFormat::RGB, m_resourceManager.fontManager().atlasData());
    m_resourceManager.fontManager().markAtlasClean();
}