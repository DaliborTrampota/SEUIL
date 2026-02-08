#pragma once

#include <glm/glm.hpp>
#include <string_view>
#include <variant>

#include "../Configuration.h"
#include "../ResourceManager.h"
#include "../font/FontManager.h"


namespace ui {
    struct DrawQuad {
        glm::ivec4 rect;
        glm::vec4 color;
        unsigned roundRadius;
        unsigned borderThickness;
        glm::vec3 borderColor;
    };

    struct DrawImage {
        glm::ivec4 rect;
        TextureHandle texture;
        float opacity;
        unsigned roundRadius;
    };

    struct DrawText {
        TextLayout layout;
        glm::vec4 color;
        AnchorPoint alignment;
    };

    using DrawCommand = std::variant<DrawQuad, DrawImage, DrawText>;
}  // namespace ui