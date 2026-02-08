#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include <LWGL/buffer/VertexLayout.h>


namespace ui::lwgl {

    enum class QuadType : uint8_t {
        Panel = 0,
        Image = 1,
        Button = 2,
        Label = 3,
        ImagePanel = 4,
        Texture = 5,
    };


    template <typename T>
    struct Quad {
        T vertices[6];
        QuadType type;
    };

    struct QuadVertex {
        glm::vec2 pos;
        glm::vec2 uv;
        unsigned int colorIndex;
        glm::uvec3 borderData;  // roundRadius, borderThickness, borderColorIndex
        static gl::VertexLayout layout() {
            return {
                sizeof(QuadVertex),
                {
                    {0, gl::VertexAttribute::Float, 2, offsetof(QuadVertex, pos)},
                    {1, gl::VertexAttribute::Float, 2, offsetof(QuadVertex, uv)},
                    {2, gl::VertexAttribute::UInt, 1, offsetof(QuadVertex, colorIndex)},
                    {3, gl::VertexAttribute::UInt, 3, offsetof(QuadVertex, borderData)},
                },
            };
        }
    };

    struct ImageVertex {
        glm::vec2 pos;
        glm::vec2 uv;
        unsigned int imageIndex;
        float opacity;
        unsigned int roundRadius;
        static gl::VertexLayout layout() {
            return {
                sizeof(ImageVertex),
                {
                    {0, gl::VertexAttribute::Float, 2, offsetof(ImageVertex, pos)},
                    {1, gl::VertexAttribute::Float, 2, offsetof(ImageVertex, uv)},
                    {2, gl::VertexAttribute::UInt, 1, offsetof(ImageVertex, imageIndex)},
                    {3, gl::VertexAttribute::Float, 1, offsetof(ImageVertex, opacity)},
                    {4, gl::VertexAttribute::UInt, 1, offsetof(ImageVertex, roundRadius)},
                },
            };
        }
    };


    struct TextVertex {
        glm::vec2 pos;
        glm::vec2 uv;
        unsigned int colorIndex;
        static gl::VertexLayout layout() {
            return {
                sizeof(TextVertex),
                {
                    {0, gl::VertexAttribute::Float, 2, offsetof(TextVertex, pos)},
                    {1, gl::VertexAttribute::Float, 2, offsetof(TextVertex, uv)},
                    {2, gl::VertexAttribute::UInt, 1, offsetof(TextVertex, colorIndex)},
                },
            };
        }
    };

    template <typename T>
    inline T makeQuad(const glm::ivec4& posSize) {
        glm::vec2 TL = {posSize.x, posSize.y};
        glm::vec2 TR = {posSize.x + posSize.z, posSize.y};
        glm::vec2 BL = {posSize.x, posSize.y + posSize.w};
        glm::vec2 BR = {posSize.x + posSize.z, posSize.y + posSize.w};

        T quad;
        // The vert shader's NDC has Y inverted
        quad.vertices[0] = {BR, glm::vec2(1.0f, 1.0f)};
        quad.vertices[1] = {TL, glm::vec2(0.0f, 0.0f)};
        quad.vertices[2] = {BL, glm::vec2(0.0f, 1.0f)};
        quad.vertices[3] = {TL, glm::vec2(0.0f, 0.0f)};
        quad.vertices[4] = {BR, glm::vec2(1.0f, 1.0f)};
        quad.vertices[5] = {TR, glm::vec2(1.0f, 0.0f)};

        return quad;
    }

}  // namespace ui::lwgl