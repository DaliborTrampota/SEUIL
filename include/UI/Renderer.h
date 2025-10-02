#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace ui {

    class Panel;
    class UIElement;

    class Renderer {
      public:
        Renderer(unsigned int fboID, const glm::ivec2& viewportSize);
        virtual ~Renderer();

        void mouseMove(const glm::ivec2& pos);
        void update(float dt);

        void setRoot(std::shared_ptr<Panel> root);
        void setViewportSize(const glm::ivec2& size);

        void renderPanel(Panel& panel);

      protected:
        void setupFBO();

      private:
        unsigned int m_fboID = 0;
        unsigned int m_fboTextureID = 0;
        glm::ivec2 m_viewportSize;
        std::shared_ptr<Panel> m_root;

        bool m_dirty = false;


        void render();

        void resize();
        void layoutElement(UIElement& element, const glm::ivec4& parentDims);
    };
}  // namespace ui