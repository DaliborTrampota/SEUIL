#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "ImageDataMgr.h"

namespace ui {

    class RendererImpl;

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

        void mouseMove(const glm::ivec2& pos);
        void update(float dt);

        void setRoot(std::shared_ptr<Panel> root);
        void setViewportSize(const glm::ivec2& size);

        void renderPanel(Panel& panel);
        void renderImage(Image& image);
        void renderButton(Button& button);

        unsigned int textureID() const;
        inline static ImageDataMgr imageDataMgr;

      protected:
        void setupFBO();

      private:
        std::unique_ptr<RendererImpl> m_impl;
        std::shared_ptr<Panel> m_root;

        bool m_dirty = false;

        void render();
        void layoutElement(UIElement& element, const glm::ivec4& parentDims);
    };
}  // namespace ui