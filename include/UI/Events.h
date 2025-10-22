#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace ui {
    enum class EventState {
        None,
        Hovered,
        Pressed,
    };


    enum class MouseBtn {
        None,
        RMB,
        LMB,
        Wheel
    };

    enum class Action {
        Move,
        Press,
        Release,
        WheelUp,
        WheelDown
    };

    struct MouseEvent {
        MouseBtn button;
        Action action;

        union {
            glm::ivec2 pos;
            float wheel;
        };
    };

    struct HoverEvent {
        bool hoverStart;
        glm::ivec2 pos;
    };

    struct ButtonEvent {
        bool pressed;
        MouseBtn button;
        glm::ivec2 pos;
    };
}  // namespace ui