#include "OpenGLRendererImpl.h"

#include <glad/glad.h>

#include <UI/detail/Quad.h>
#include <UI/elements/Button.h>
#include <UI/elements/Image.h>
#include <UI/elements/Label.h>
#include <UI/elements/Panel.h>
#include <UI/font/FontLoader.h>

#include <LWGL/render/Shader.h>
#include <LWGL/texture/ImageData.h>
#include <LWGL/texture/Texture2D.h>
#include <LWGL/texture/TextureBase.h>


#include <utility>  // for std::swap

using namespace ui;

namespace {
    void adjustAlignment(AnchorPoint alignment, const glm::vec4& posSize, glm::vec2& vertPos) {
        bool top = static_cast<uint8_t>(alignment & AnchorPoint::Top);
        bool bottom = static_cast<uint8_t>(alignment & AnchorPoint::Bottom);
        bool left = static_cast<uint8_t>(alignment & AnchorPoint::Left);
        bool right = static_cast<uint8_t>(alignment & AnchorPoint::Right);
        // no need to check for mid, because it will be handled by the x and y checks

        bool x = left || right;
        bool y = top || bottom;

        if (!x)
            vertPos.x -= posSize.z / 2;

        if (!y)
            vertPos.y += posSize.w / 2;

        if (top)
            vertPos.y += posSize.w;

        if (right)
            vertPos.x -= posSize.z;
    }
}  // namespace


unsigned int OpenGLRendererImpl::findOrStoreColor(const glm::vec4& color) {
    const std::vector<glm::vec4>& data = m_shaderColors.data();
    auto it = std::find(data.begin(), data.end(), color);
    if (it == data.end()) {
        m_shaderColors.add(color);
        return data.size() - 1;
    }
    return std::distance(data.begin(), it);
}

OpenGLRendererImpl::OpenGLRendererImpl(const glm::ivec2& viewportSize)
    : m_material("resources/shaders/SEUIL.vert", "resources/shaders/SEUIL.frag", "SEUIL"),
      m_textMaterial(
          "resources/shaders/SEUIL_TEXT.vert", "resources/shaders/SEUIL_TEXT.frag", "SEUIL_TEXT"
      ),
      m_attributes(GL_DYNAMIC_DRAW),
      m_textAttributes(GL_DYNAMIC_DRAW),
      m_fbo(),
      m_outputTexture(0),
      m_shaderColors(1),
      m_fontAtlas(FontAtlas::createDynamic()) {
    m_viewportSize = viewportSize;

    // Check for ARB_bindless_texture support
    if (glGetTextureHandleARB == nullptr || glMakeTextureHandleResidentARB == nullptr) {
        printf("ERROR: GL_ARB_bindless_texture functions not loaded!\n");
    } else {
        printf("GL_ARB_bindless_texture functions are available\n");
    }

    setupFBO();
    m_material.use();
    m_material.setVec2("uScreenSize", (glm::vec2)viewportSize);
    m_textMaterial.use();
    m_textMaterial.setVec2("uScreenSize", (glm::vec2)viewportSize);
    m_textMaterial.setInt("uMSDF", 0);
    m_textMaterial.setFloat("pxRange", FontLoader::PIXEL_RANGE);

    m_attributes.create();
    m_textAttributes.create();
    m_shaderColors.create({});

    m_textTexture.create(gl::Settings::LinearClampToEdge());  // linear interpolation
}

void OpenGLRendererImpl::resize(const glm::ivec2& newSize) {
    // Only resize if size actually changed
    if (m_viewportSize == newSize) {
        return;
    }

    m_viewportSize = newSize;

    m_outputTexture.bind();
    m_outputTexture.loadRaw(newSize.x, newSize.y, 4, gl::ImageFormat::RGBA, nullptr);
    m_rbo.create(newSize.x, newSize.y, GL_DEPTH24_STENCIL8, 0);

    // Re-attach RBO to FBO since storage was recreated
    m_fbo.bind();
    m_fbo.attachRenderBuffer(m_rbo, gl::FBOAttachment::DepthStencil, gl::FBO::Target::ReadDraw);
    assert(m_fbo.checkCompleteness() == 0);
    m_fbo.unbind();

    m_material.use();
    m_material.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
    m_textMaterial.use();
    m_textMaterial.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
}

void OpenGLRendererImpl::setupFBO() {
    m_outputTexture.create(gl::Settings::LinearClampToEdge());
    m_outputTexture.loadRaw(m_viewportSize.x, m_viewportSize.y, 4, gl::ImageFormat::RGBA, nullptr);

    m_fbo.bind();
    m_fbo.bindTexture(gl::FBOAttachment::Color, m_outputTexture.id());

    m_rbo.create(m_viewportSize.x, m_viewportSize.y, GL_DEPTH24_STENCIL8, 0);
    m_fbo.attachRenderBuffer(m_rbo, gl::FBOAttachment::DepthStencil, gl::FBO::Target::ReadDraw);
    assert(m_fbo.checkCompleteness() == 0);

    m_fbo.clearActive({0.0f, 0.0f, 0.0f, 0.0f}, 1.0f, 0);

    m_fbo.unbind();
    //m_outputTexture.unbind();
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void OpenGLRendererImpl::beforeRender() {
    m_fbo.bind();
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Clear to fully transparent
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_bindlessTextures.use();
    m_bindlessTextures.update();
    m_shaderColors.bind();
}

void OpenGLRendererImpl::afterRender() {
    m_attributes.clear();
    m_textAttributes.clear();
    m_bindlessTextures.unload();  // Unload because we dont render UI every frame
    m_shaderColors.unbind();

    // Restore OpenGL state
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void OpenGLRendererImpl::render() {
    beforeRender();

    if (m_attributes.length() > 0) {
        m_material.use();
        m_attributes.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_attributes.length());
    }

    if (m_textAttributes.length() > 0) {
        m_textMaterial.use();
        m_textAttributes.bind();
        m_textMaterial.setInt("uMSDF", 0);
        m_textMaterial.setFloat("pxRange", FontLoader::PIXEL_RANGE);
        m_textTexture.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_textAttributes.length());
    }

    afterRender();
}

detail::Quad OpenGLRendererImpl::makeQuad(const glm::ivec4& posSize) const {
    glm::vec2 TL = {posSize.x, posSize.y};
    glm::vec2 TR = {posSize.x + posSize.z, posSize.y};
    glm::vec2 BL = {posSize.x, posSize.y + posSize.w};
    glm::vec2 BR = {posSize.x + posSize.z, posSize.y + posSize.w};

    detail::Quad quad;
    // The vert shader's NDC has Y inverted
    quad.vertices[0] = {BR, glm::vec2(1.0f, 1.0f)};
    quad.vertices[1] = {TL, glm::vec2(0.0f, 0.0f)};
    quad.vertices[2] = {BL, glm::vec2(0.0f, 1.0f)};
    quad.vertices[3] = {TL, glm::vec2(0.0f, 0.0f)};
    quad.vertices[4] = {BR, glm::vec2(1.0f, 1.0f)};
    quad.vertices[5] = {TR, glm::vec2(1.0f, 0.0f)};

    return quad;
}

detail::TextQuad OpenGLRendererImpl::makeTextQuad(
    const glm::ivec4& posSize, const glm::vec4& uv
) const {
    glm::vec2 TL = {posSize.x, posSize.y};
    glm::vec2 TR = {posSize.x + posSize.z, posSize.y};
    glm::vec2 BL = {posSize.x, posSize.y + posSize.w};
    glm::vec2 BR = {posSize.x + posSize.z, posSize.y + posSize.w};

    glm::vec2 uvTL = {uv.x, uv.y};
    glm::vec2 uvTR = {uv.z, uv.y};
    glm::vec2 uvBL = {uv.x, uv.w};
    glm::vec2 uvBR = {uv.z, uv.w};

    detail::TextQuad quad;
    quad.vertices[0] = {BR, uvBR};
    quad.vertices[1] = {TL, uvTL};
    quad.vertices[2] = {BL, uvBL};
    quad.vertices[3] = {TL, uvTL};
    quad.vertices[4] = {BR, uvBR};
    quad.vertices[5] = {TR, uvTR};
    return quad;
}


void OpenGLRendererImpl::renderPanel(const Panel& panel) {
    const auto& style = panel.style_c();
    if (style.backgroundColor.a == 0.f)
        return;

    detail::Quad quad = makeQuad(panel.m_calculatedDims);

    unsigned int colorIndex = findOrStoreColor(style.backgroundColor);
    unsigned int borderColorIndex = findOrStoreColor({style.borderColor, 1.f});

    for (auto& vert : quad.vertices) {
        vert.colorIndex = colorIndex;
        vert.type = detail::QuadType::Panel;
        vert.borderData = {style.roundRadius, style.borderThickness, borderColorIndex};
        m_attributes.move(std::move(vert));
    }
}

void OpenGLRendererImpl::renderImage(
    const Image& image
) {  // TODO dont add the color into SSBO since its always 1.0f
    const auto& style = image.style_c();
    if (style.opacity == 0.f)
        return;

    detail::Quad quad = makeQuad(image.m_calculatedDims);

    size_t idx = image.index();

    for (auto& vert : quad.vertices) {
        vert.borderData = {style.roundRadius, 0, 0};
        vert.type = detail::QuadType::Image;
        vert.data = idx;
        vert.colorIndex = style.opacity;
        m_attributes.move(std::move(vert));
    }
}

void OpenGLRendererImpl::renderButton(const Button& button) {
    const auto& style = button.style_c();
    EventState state = button.state();

    detail::Quad quad = makeQuad(button.m_calculatedDims);

    Style<Button>::State stateStyle = state == EventState::Hovered   ? style.hovered
                                      : state == EventState::Pressed ? style.pressed
                                                                     : style.normal;

    unsigned int bgColorIndex = findOrStoreColor(stateStyle.background);
    unsigned int textColorIndex = findOrStoreColor({stateStyle.text, 1.f});
    unsigned int borderColorIndex = findOrStoreColor({stateStyle.border, 1.f});

    for (auto& vert : quad.vertices) {
        vert.borderData = {style.roundRadius, style.borderThickness, borderColorIndex};
        vert.type = detail::QuadType::Button;
        vert.data = button.isHovered() ? 1 : button.isPressed() ? 2 : 0;
        vert.colorIndex = bgColorIndex;
        m_attributes.move(std::move(vert));
    }
    //render label
}

void OpenGLRendererImpl::renderLabel(const Label& label) {
    const auto& style = label.style_c();
    if (style.color.a == 0.f)
        return;

    const AnchorPoint& alignment = label.alignment();

    // Get font metrics to calculate scale
    const msdf_atlas::FontGeometry* fontGeometry =
        m_fontLoader.getFontGeometry(style.font, style.fontSize);
    if (!fontGeometry) {
        throw std::runtime_error("Failed to get font geometry");
    }

    const msdfgen::FontMetrics& metrics = fontGeometry->getMetrics();
    double scale = style.fontSize / metrics.emSize;
    const unsigned char* data = m_fontAtlas.data();

    unsigned int colorIndex = findOrStoreColor(style.color);
    glm::vec4 labelPos = label.m_calculatedDims;

    float advance = 0.f;
    glm::vec2 atlasSize{m_fontAtlas.width(), m_fontAtlas.height()};

    std::vector<detail::UITextVertex> vertices;
    vertices.reserve(label.m_textCache.size() * 6);


    float maxHeight = 0.f;

    for (const auto& glyph : label.m_textCache) {
        if (glyph->isWhitespace()) {
            advance += glyph->getAdvance() * scale;
            continue;
        }
        double l, b, r, t;
        glyph->getQuadPlaneBounds(l, b, r, t);

        glm::vec4 posSize = {
            l * scale,
            -t * scale,  // invert Y: use -t for top
            (r - l) * scale,
            (t - b) * scale
        };

        maxHeight = std::max(maxHeight, posSize.w);

        posSize += labelPos;
        posSize.x += advance;

        advance += glyph->getAdvance() * scale;

        glm::dvec4 uv;
        glyph->getQuadAtlasBounds(uv.x, uv.y, uv.z, uv.w);

        uv.x /= atlasSize.x;
        uv.y /= atlasSize.y;
        uv.z /= atlasSize.x;
        uv.w /= atlasSize.y;

        std::swap(uv.y, uv.w);  // Flip UV vertically (swap top and bottom)

        detail::TextQuad quad = makeTextQuad(posSize, uv);
        for (auto& vert : quad.vertices) {
            vert.colorIndex = colorIndex;
            vertices.emplace_back(std::move(vert));
        }
    }

    const_cast<Label&>(label).m_calculatedDims.z = advance;
    const_cast<Label&>(label).m_calculatedDims.w = maxHeight;

    for (auto& vert : vertices) {
        adjustAlignment(alignment, label.m_calculatedDims, vert.pos);
        m_textAttributes.move(std::move(vert));
    }
}

void OpenGLRendererImpl::loadImage(Image& image) {
    const ImageData* textureData = image.textureData();

    size_t idx = m_bindlessTextures.add(
        textureData->data,
        textureData->width,
        textureData->height,
        textureData->channels,
        image.style_c().pixelated
    );

    printf(
        "Loaded image at index %zu, size: %dx%d, channels: %d\n",
        idx,
        textureData->width,
        textureData->height,
        textureData->channels
    );

    image.index(idx);
}

void OpenGLRendererImpl::loadText(Label& label) {
    const auto& style = label.style_c();

    int startIndex, count;
    m_fontLoader.loadFont(style.font);
    std::vector<const msdf_atlas::GlyphGeometry*> glyphs =
        m_fontLoader.generateText(label.text(), style.fontSize, startIndex, count);


    m_fontAtlas.addGlyphs(m_fontLoader.glyphs().data() + startIndex, count);
    //m_fontAtlas = m_fontLoader.packToAtlas(); // Switch to static atlas

    const unsigned char* data = m_fontAtlas.data();
    int atlasWidth = m_fontAtlas.width();
    int atlasHeight = m_fontAtlas.height();

    m_textTexture.bind();

    if (atlasWidth != m_textTexture.width() || atlasHeight != m_textTexture.height()) {
        m_textTexture.loadRaw(
            atlasWidth, atlasHeight, 3, gl::ImageFormat::RGB, const_cast<unsigned char*>(data)
        );
    } else {
        m_textTexture.update(gl::ImageFormat::RGB, const_cast<unsigned char*>(data));
    }

    label.m_textCache = glyphs;
    label.m_dirty = false;
}