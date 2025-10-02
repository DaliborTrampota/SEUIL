#pragma once

#include <concepts>
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

    struct Base {
        float x, y;
        glm::ivec2 calc;
    };

    template <typename U, typename V>
    struct Pos {};

    struct Rel;
    struct Abs;
    struct Auto;

    template <typename T>
    concept ValidSizeT = std::same_as<T, Abs> || std::same_as<T, Rel> || std::same_as<T, Auto>;

    template <typename T>
    concept ValidPosT = std::same_as<T, Abs> || std::same_as<T, Rel>;


    template <>
    struct Pos<Rel, Rel> : public Base {};

    template <>
    struct Pos<Abs, Abs> : public Base {};

    template <>
    struct Pos<Abs, Rel> : public Base {};

    template <>
    struct Pos<Rel, Abs> : public Base {};


    template <typename U, typename V>
    struct Size {};

    template <>
    struct Size<Abs, Abs> : public Base {};

    template <>
    struct Size<Rel, Rel> : public Base {};

    template <>
    struct Size<Abs, Rel> : public Base {};

    template <>
    struct Size<Rel, Abs> : public Base {};

    template <>
    struct Size<Auto, Auto> : public Base {
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