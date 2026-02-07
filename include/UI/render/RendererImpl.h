#pragma once

#include <glm/glm.hpp>

#include "../Configuration.h"
#include "DrawCommands.h"


namespace ui {
    class ResourceManager;

    class RendererImpl {
      public:
        virtual ~RendererImpl() = default;

        glm::ivec2 viewportSize() const { return m_viewportSize; }

        virtual void resize(const glm::ivec2& viewportSize) = 0;
        virtual void submitCommand(const DrawCommand& command) = 0;
        virtual void flush() = 0;

        virtual ResourceManager& resourceManager() = 0;

        virtual NativeOutputHandle outputHandle() const = 0;

      protected:
        glm::ivec2 m_viewportSize;
    };
}  // namespace ui