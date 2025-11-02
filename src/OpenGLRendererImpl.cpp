#include "OpenGLRendererImpl.h"

#include <glad/glad.h>

#include <UI/elements/Button.h>
#include <UI/elements/Image.h>
#include <UI/elements/Label.h>
#include <UI/elements/Panel.h>

#include "detail/Quad.h"
#include "detail/Shader.h"

#include <UI/font/FontLoader.h>

#include <LWGL/render/Shader.h>
#include <LWGL/texture/Texture2D.h>

#include <utility>  // for std::swap

using namespace ui;


unsigned int OpenGLRendererImpl::findOrStoreColor(const glm::vec4& color) {
    const std::vector<glm::vec4>& data = m_shaderColors.data();
    auto it = std::find(data.begin(), data.end(), color);
    if (it == data.end()) {
        m_shaderColors.add(color);
        return data.size() - 1;
    }
    return std::distance(data.begin(), it);
}

OpenGLRendererImpl::OpenGLRendererImpl(unsigned int fboID, const glm::ivec2& viewportSize)
    : m_material("resources/shaders/SEUIL.vert", "resources/shaders/SEUIL.frag", "SEUIL"),
      m_textMaterial(
          "resources/shaders/SEUIL_TEXT.vert", "resources/shaders/SEUIL_TEXT.frag", "SEUIL_TEXT"
      ),
      m_attributes(GL_DYNAMIC_DRAW),
      m_textAttributes(GL_DYNAMIC_DRAW),
      m_fboID(fboID),
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
    m_textMaterial.setFloat("pxRange", FontAtlas::PIXEL_RANGE);

    m_attributes.create();
    m_textAttributes.create();
    m_shaderColors.create({});

    m_textTexture.create(gl::Settings());  // linear interpolation
}

OpenGLRendererImpl::~OpenGLRendererImpl() {
    if (m_fboTextureID) {
        glDeleteTextures(1, &m_fboTextureID);
    }
}

void OpenGLRendererImpl::resize(const glm::ivec2& newSize) {
    m_viewportSize = newSize;
    setupFBO();

    m_material.use();
    m_material.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
    m_textMaterial.use();
    m_textMaterial.setVec2("uScreenSize", (glm::vec2)m_viewportSize);
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

void OpenGLRendererImpl::beforeRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
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
        m_textMaterial.setFloat("pxRange", FontAtlas::PIXEL_RANGE);
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
    quad.vertices[0] = {BR, glm::vec2(1.0f, 0.0f)};
    quad.vertices[1] = {TL, glm::vec2(0.0f, 1.0f)};
    quad.vertices[2] = {BL, glm::vec2(0.0f, 0.0f)};
    quad.vertices[3] = {TL, glm::vec2(0.0f, 1.0f)};
    quad.vertices[4] = {BR, glm::vec2(1.0f, 0.0f)};
    quad.vertices[5] = {TR, glm::vec2(1.0f, 1.0f)};

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
    //label.m_calculatedDims.w =

    for (auto& vert : vertices)
        m_textAttributes.move(std::move(vert));
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

    m_textTexture.bind();
    m_textTexture.loadRaw(
        m_fontAtlas.width(),
        m_fontAtlas.height(),
        3,
        gl::ImageFormat::RGB,
        const_cast<unsigned char*>(data)
    );

    label.m_textCache = glyphs;
    label.m_dirty = false;
}