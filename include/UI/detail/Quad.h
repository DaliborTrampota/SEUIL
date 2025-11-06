#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include <glad/glad.h>

#include <LWGL/buffer/VertexLayout.h>


namespace ui::detail {

    enum class QuadType : uint8_t {
        Panel = 0,
        Image = 1,
        Button = 2,
        Label = 3,
    };


    struct UIVertex {
        glm::vec2 pos;
        glm::vec2 uv;
        unsigned int colorIndex;
        QuadType type;
        glm::uvec3 borderData;
        unsigned int data = 0;

        static gl::VertexLayout layout() {
            return {
                sizeof(UIVertex),
                {
                    {0, gl::VertexAttribute::Float, 2, offsetof(UIVertex, pos)},
                    {1, gl::VertexAttribute::Float, 2, offsetof(UIVertex, uv)},
                    {2, gl::VertexAttribute::UInt, 1, offsetof(UIVertex, colorIndex)},
                    {3, gl::VertexAttribute::UInt, 1, offsetof(UIVertex, type)},
                    {4, gl::VertexAttribute::UInt, 3, offsetof(UIVertex, borderData)},
                    {5, gl::VertexAttribute::UInt, 1, offsetof(UIVertex, data)},
                },
            };
        }
    };

    struct UITextVertex {
        glm::vec2 pos;
        glm::vec2 uv;
        unsigned int colorIndex;
        static gl::VertexLayout layout() {
            return {
                sizeof(UITextVertex),
                {
                    {0, gl::VertexAttribute::Float, 2, offsetof(UITextVertex, pos)},
                    {1, gl::VertexAttribute::Float, 2, offsetof(UITextVertex, uv)},
                    {2, gl::VertexAttribute::UInt, 1, offsetof(UITextVertex, colorIndex)},
                },
            };
        }
    };

    struct Quad {
        UIVertex vertices[6];
        QuadType type;
    };

    struct TextQuad {
        UITextVertex vertices[6];
    };

}  // namespace ui::detail