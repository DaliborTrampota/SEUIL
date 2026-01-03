#pragma once

#include <concepts>
#include <filesystem>
#include <glm/glm.hpp>
#include <string>


namespace ui {

    enum class CursorType {
        Default,
        Pointer,
        Type,
    };

    enum class AnchorPoint : uint8_t {
        None = 0,
        Top = 1,
        Bottom = 2,
        Left = 4,
        Right = 8,
        Mid = 16,
        TopLeft = Top | Left,
        TopRight = Top | Right,
        BottomLeft = Bottom | Left,
        BottomRight = Bottom | Right,
    };

    inline AnchorPoint operator&(AnchorPoint lhs, AnchorPoint rhs) {
        return static_cast<AnchorPoint>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
    }

    struct Base {
        float x, y;
    };

    template <typename U, typename V>
    struct Pos;

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
    struct Size;

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
    struct Style;

    class Panel;
    class ImagePanel;
    class Image;
    class Button;
    class Label;

    using ColorRGBA = glm::vec4;
    using ColorRGB = glm::vec3;

    template <>
    struct Style<Panel> {
        ColorRGBA backgroundColor;
        unsigned int roundRadius;
        unsigned int borderThickness;
        ColorRGB borderColor;  // Alpha is taken from bg color
    };

    template <>
    struct Style<ImagePanel> {
        std::filesystem::path backgroundImage;
        float opacity = 1.0f;
        bool pixelated = false;
        unsigned int roundRadius = 0;
    };

    template <>
    struct Style<Image> {
        unsigned int roundRadius = 0;
        float opacity = 1.0f;
        bool pixelated = false;
    };

    template <>
    struct Style<Button> {
        struct State {
            ColorRGBA background;
            // Alpha is taken from bg color
            ColorRGB text;
            ColorRGB border;
        };

        State normal;
        State hovered;
        State pressed;

        unsigned int roundRadius;
        unsigned int borderThickness;
    };

    template <>
    struct Style<Label> {
        ColorRGBA color = {1.f, 1.f, 1.f, 1.f};
        std::string font = "resources/fonts/ARIAL.TTF";
        uint8_t fontSize = 16;
    };
}  // namespace ui