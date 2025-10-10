#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include <glad/glad.h>

namespace ui::detail {

    enum class QuadType : uint8_t {
        Panel = 0,
        Image = 1,
    };

    struct UIVertex {
        glm::vec2 pos;
        glm::vec2 uv;
        glm::vec4 color;
        unsigned int roundness;
        QuadType type;
        unsigned int borderThickness = 0;
        glm::vec3 borderColor = {0, 0, 0};
        unsigned int data = 0;
    };

    struct Quad {
        UIVertex vertices[6];
        QuadType type;

        static void attributes() {
            int stride = sizeof(detail::UIVertex);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);  // pos
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(uintptr_t(offsetof(detail::UIVertex, uv)))
            );  // uv
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(
                2,
                4,
                GL_FLOAT,
                GL_FALSE,
                stride,
                (void*)(uintptr_t(offsetof(detail::UIVertex, color)))
            );  // color
            glEnableVertexAttribArray(2);
            glVertexAttribIPointer(
                3,
                1,
                GL_UNSIGNED_INT,
                stride,
                (void*)(uintptr_t(offsetof(detail::UIVertex, roundness)))
            );  // roundness
            glEnableVertexAttribArray(3);
            glVertexAttribIPointer(
                4, 1, GL_UNSIGNED_INT, stride, (void*)(uintptr_t(offsetof(detail::UIVertex, type)))
            );  // type
            glEnableVertexAttribArray(4);

            glVertexAttribIPointer(
                5,
                1,
                GL_UNSIGNED_INT,
                stride,
                (void*)(uintptr_t(offsetof(detail::UIVertex, borderThickness)))
            );  // borderThickness
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(
                6,
                3,
                GL_FLOAT,
                GL_FALSE,
                stride,
                (void*)(uintptr_t(offsetof(detail::UIVertex, borderColor)))
            );  // borderColor
            glEnableVertexAttribArray(6);

            glVertexAttribIPointer(
                7, 1, GL_UNSIGNED_INT, stride, (void*)(uintptr_t(offsetof(detail::UIVertex, data)))
            );  // data
            glEnableVertexAttribArray(7);
        }
    };

}  // namespace ui::detail