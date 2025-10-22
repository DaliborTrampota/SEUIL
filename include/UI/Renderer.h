#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>


#include "ImageDataMgr.h"

namespace ui {

    class RendererImpl;
    struct MouseEvent;

    class UIElement;
    class Panel;
    class Image;

    enum class RendererType {
        OpenGL,
    };

    class Renderer {
      public:
        Renderer(RendererType type, const glm::ivec2& viewportSize);
        virtual ~Renderer();

        void mouseEvent(const MouseEvent& event);
        void update(float dt);

        void setRoot(std::shared_ptr<Panel> root);
        void setViewportSize(const glm::ivec2& size);

        void renderPanel(Panel& panel);
        void renderImage(Image& image);
        void renderButton(Button& button);

        unsigned int textureID() const;
        inline static ImageDataMgr imageDataMgr;

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
    };
}  // namespace ui