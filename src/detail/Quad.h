#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace ui::detail {

    enum class QuadType : uint8_t {
        Panel,
    };

    struct UIVertex {
        glm::vec2 pos;
        glm::vec2 uv;
        glm::vec4 color;
        float roundness;
        QuadType type;
    };

    struct Quad {
        UIVertex vertices[6];
        QuadType type;
    };

}  // namespace ui::detail