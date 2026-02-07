#include <UI/Renderer.h>

#include <UI/Events.h>
#include <UI/elements/Button.h>
#include <UI/elements/Image.h>
#include <UI/elements/ImagePanel.h>
#include <UI/elements/Label.h>
#include <UI/elements/Panel.h>
#include <UI/elements/Texture.h>


#include "backends/LWGL/LWGLRendererImpl.h"


#include <glad/glad.h>

using namespace ui;

Renderer::Renderer(RendererType type, const glm::ivec2& viewportSize) {
    switch (type) {
        case RendererType::LWGL: m_impl = std::make_unique<LWGLRendererImpl>(viewportSize); break;
        case RendererType::OpenGL:
            // m_impl = std::make_unique<OpenGLRendererImpl>(viewportSize);
            break;
    }
}

Renderer::~Renderer() {}

void Renderer::registerCursorFunction(std::function<void(CursorType)> func) {
    setCursorFunc = func;
}

void Renderer::mouseEvent(const MouseEvent& event) {
    m_root->mouseEvent(event);
}

void Renderer::update(float dt) {
    if (s_dirty) {
        if (m_root) {
            glm::ivec2 viewportSize = m_impl->viewportSize();
            layoutElement(*m_root, {0, 0, viewportSize.x, viewportSize.y});
            render();
        }
        s_dirty = false;
    }
}

void Renderer::setRoot(std::shared_ptr<Panel> root) {
    m_root = root;
    s_dirty = true;
}

void Renderer::setViewportSize(const glm::ivec2& size) {
    m_impl->resize(size);
    s_dirty = true;
}


void Renderer::layoutElement(UIElement& element, const glm::ivec4& parentDims) {
    const glm::ivec4 calculatedDims = element.calculateSizeAndPosition(parentDims);

    if (auto* panel = dynamic_cast<Panel*>(&element)) {
        for (auto& child : panel->children()) {
            layoutElement(*child, calculatedDims);
        }
    }
    if (auto* imagePanel = dynamic_cast<ImagePanel*>(&element)) {
        for (auto& child : imagePanel->children()) {
            layoutElement(*child, calculatedDims);
        }
    }
}

void Renderer::render() {
    if (m_root) {
        renderElements(m_root.get());
        m_impl->flush();
    }
}

NativeOutputHandle Renderer::outputHandle() const {
    return m_impl->outputHandle();
}

TextureHandle Renderer::loadTexture(const std::string& path, bool pixelated) {
    return m_impl->resourceManager().loadTexture(path, pixelated);
}

TextureHandle Renderer::registerNativeTexture(void* nativeHandle) {
    return m_impl->resourceManager().registerNativeTexture(nativeHandle);
}

void Renderer::unloadTexture(TextureHandle handle) {
    m_impl->resourceManager().unloadTexture(handle);
}

FontHandle Renderer::loadFont(const std::string& path) {
    return m_impl->resourceManager().loadFont(path);
}

void Renderer::unloadFont(FontHandle handle) {
    m_impl->resourceManager().unloadFont(handle);
}


void Renderer::renderElements(UIElement* element) {
    if (auto* imagePanel = dynamic_cast<ImagePanel*>(element)) {
        Style<ImagePanel> style = imagePanel->style_c();

        if (imagePanel->m_dirty) {
            imagePanel->m_textureHandle = m_impl->resourceManager().loadTexture(
                style.backgroundImage.string(), style.pixelated
            );
            imagePanel->m_dirty = false;
        }

        DrawImage imageCmd = {
            .rect = element->absolutePositionAndSize(),
            .texture = imagePanel->resourceHandle(),
            .opacity = 1.0f,
            .roundRadius = style.roundRadius,
        };
        m_impl->submitCommand(imageCmd);

        for (auto& child : imagePanel->children()) {
            renderElements(child.get());
        }


    } else if (auto* panel = dynamic_cast<Panel*>(element)) {
        Style<Panel> style = panel->style_c();
        DrawQuad quadCmd = {
            .rect = element->absolutePositionAndSize(),
            .color = style.backgroundColor,
            .roundRadius = style.roundRadius,
            .borderThickness = style.borderThickness,
            .borderColor = style.borderColor,
        };
        m_impl->submitCommand(quadCmd);

        for (auto& child : panel->children()) {
            renderElements(child.get());
        }


    } else if (auto* image = dynamic_cast<Image*>(element)) {
        Style<Image> style = image->style_c();

        if (image->m_dirty) {
            image->m_textureHandle = m_impl->resourceManager().loadTexture(
                image->m_sourcePath.string(), style.pixelated
            );
            image->m_dirty = false;
        }

        DrawImage imageCmd = {
            .rect = element->absolutePositionAndSize(),
            .texture = image->resourceHandle(),
            .opacity = style.opacity,
            .roundRadius = style.roundRadius,
        };
        m_impl->submitCommand(imageCmd);


    } else if (auto* label = dynamic_cast<Label*>(element)) {
        Style<Label> style = label->style_c();

        if (label->m_dirty) {
            // TODO font handle and layout
            label->m_textLayout = m_impl->resourceManager().layoutText(
                label->text(),
                label->fontHandle(),
                style.fontSize,
                label->absolutePositionAndSize(),
                label->alignment()
            );
            label->m_dirty = false;
        }

        const glm::ivec4& posSize = element->absolutePositionAndSize();
        DrawText textCmd = {
            .position = glm::ivec2(posSize.x, posSize.y),
            .text = label->text(),
            .font = label->fontHandle(),
            .color = style.color,
            .alignment = label->alignment(),
        };
        m_impl->submitCommand(textCmd);


    } else if (auto* texture = dynamic_cast<Texture*>(element)) {
        Style<Texture> style = texture->style_c();
        DrawImage imageCmd = {
            .rect = element->absolutePositionAndSize(),
            .texture = texture->resourceHandle(),
            .opacity = style.opacity,
            .roundRadius = style.roundRadius,
        };
        m_impl->submitCommand(imageCmd);


    } else if (auto* button = dynamic_cast<Button*>(element)) {
        Style<Button> style = button->style_c();
        EventState state = button->state();
        Style<Button>::State stateStyle = state == EventState::Hovered   ? style.hovered
                                          : state == EventState::Pressed ? style.pressed
                                                                         : style.normal;

        DrawQuad quadCmd = {
            .rect = element->absolutePositionAndSize(),
            .color = stateStyle.background,
            .roundRadius = style.roundRadius,
            .borderThickness = style.borderThickness,
            .borderColor = stateStyle.border,
        };
        m_impl->submitCommand(quadCmd);
    }
}