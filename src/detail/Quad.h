#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include <glad/glad.h>

namespace {
    struct Attr {
        enum Type {
            Float,
            Int,
            UInt,
        };
        int loc;
        int size;
        Type type;
        int offset;
    };
}  // namespace

namespace ui::detail {

    enum class QuadType : uint8_t {
        Panel = 0,
        Image = 1,
        Button = 2,
    };

    struct UIVertex {
        glm::vec2 pos;
        glm::vec2 uv;
        unsigned int colorIndex;
        QuadType type;
        glm::uvec3 borderData;
        unsigned int data = 0;
    };

    struct Quad {
        UIVertex vertices[6];
        QuadType type;

        static constexpr Attr layout[] = {
            {0, 2, Attr::Type::Float, 0},
            {1, 2, Attr::Type::Float, offsetof(UIVertex, uv)},
            {2, 1, Attr::Type::UInt, offsetof(UIVertex, colorIndex)},
            {3, 1, Attr::Type::UInt, offsetof(UIVertex, type)},
            {4, 3, Attr::Type::UInt, offsetof(UIVertex, borderData)},
            {5, 1, Attr::Type::UInt, offsetof(UIVertex, data)},
        };

        static void attributes() {
            int stride = sizeof(detail::UIVertex);


            for (const auto& attr : layout) {
                if (attr.type == Attr::Type::Float) {
                    glVertexAttribPointer(
                        attr.loc,
                        attr.size,
                        GL_FLOAT,
                        GL_FALSE,
                        stride,
                        (void*)(uintptr_t(attr.offset))
                    );
                } else {
                    glVertexAttribIPointer(
                        attr.loc,
                        attr.size,
                        attr.type == Attr::Type::Int ? GL_INT : GL_UNSIGNED_INT,
                        stride,
                        (void*)(uintptr_t(attr.offset))
                    );
                }
                glEnableVertexAttribArray(attr.loc);
            }
        }
    };

}  // namespace ui::detail