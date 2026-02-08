#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>

#include "Configuration.h"
#include "ResourceManager.h"
#include "font/FontManager.h"

namespace ui {

    class RendererImpl;
    struct MouseEvent;

    class UIElement;
    class Panel;
    class Image;
    class ImagePanel;
    class Button;
    class Label;
    class Texture;


    class Renderer {
      public:
        Renderer(RendererType type, FontManagerType fontManagerType, const glm::ivec2& viewportSize);
        virtual ~Renderer();

        void mouseEvent(const MouseEvent& event);
        void update(float dt);

        void setRoot(std::shared_ptr<Panel> root);
        void setViewportSize(const glm::ivec2& size);

      public:
        TextureHandle loadTexture(const std::string& path, bool pixelated = false);
        TextureHandle registerNativeTexture(void* nativeHandle);
        void unloadTexture(TextureHandle handle);

        FontHandle loadFont(const std::string& path);
        void unloadFont(FontHandle handle);

      public:
        NativeOutputHandle outputHandle() const;

        static void markDirty() { s_dirty = true; }
        static CursorType currentCursor() { return s_currentCursor; }
        static void setCurrentCursor(CursorType cursor) { s_currentCursor = cursor; }
        static void registerCursorFunction(std::function<void(CursorType)> func);
        inline static std::function<void(CursorType)> setCursorFunc = nullptr;


      protected:
        void setupFBO();

      private:
        std::unique_ptr<RendererImpl> m_impl;
        std::shared_ptr<Panel> m_root;

        inline static bool s_dirty = false;
        inline static CursorType s_currentCursor = CursorType::Default;

        void render();
        void layoutElement(UIElement& element, const glm::ivec4& parentDims);
        void renderElements(UIElement* element);
    };
}  // namespace ui