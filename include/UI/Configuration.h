#pragma once

#include <glm/glm.hpp>

namespace ui {

    enum class AnchorPoint {
        None,
        TopLeft,
        TopRight,
        MidTop,
        MidLeft,
        Mid,
        MidRight,
        MidBottom,
        BottomLeft,
        BottomRight,
    };

    template <typename U, typename V>
    struct Pos {};

    struct PosBase {
        float x, y;
    };

    struct Rel;
    struct Abs;
    struct Auto;

    template <>
    struct Pos<Rel, Rel> : public PosBase {};

    template <>
    struct Pos<Abs, Abs> : public PosBase {};

    template <>
    struct Pos<Abs, Rel> : public PosBase {};

    template <>
    struct Pos<Rel, Abs> : public PosBase {};


    template <typename U, typename V>
    struct Size {};

    struct SizeBase {
        float x, y;
        float calcX, calcY;
    };

    template <>
    struct Size<Abs, Abs> : public SizeBase {};

    template <>
    struct Size<Rel, Rel> : public SizeBase {};

    template <>
    struct Size<Abs, Rel> : public SizeBase {};

    template <>
    struct Size<Rel, Abs> : public SizeBase {};

    template <>
    struct Size<Auto, Auto> : public SizeBase {
    };  // doesnt matter that we waste space with x and y, because it will be in union anyway and the space is reserved anyway.


    template <typename T>
    struct Style {};

    class Panel;
    using ColorRGBA = glm::vec4;
    using ColorRGB = glm::vec3;

    template <>
    struct Style<Panel> {
        ColorRGBA backgroundColor;
    };
}  // namespace ui