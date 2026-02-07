#include <UI/elements/Button.h>

#include <UI/Renderer.h>

using namespace ui;

bool Button::isPressed() const {
    return m_eventState == EventState::Pressed;
}

bool Button::isHovered() const {
    return m_eventState == EventState::Hovered;
}

void Button::mouseEvent(const MouseEvent& event) {
    EventState prevState = m_eventState;
    UIElement::mouseEvent(event);


    if (Renderer::currentCursor() != CursorType::Pointer && m_eventState == EventState::Hovered) {
        Renderer::setCursorFunc(CursorType::Pointer);
        Renderer::setCurrentCursor(CursorType::Pointer);
    } else if (Renderer::currentCursor() != CursorType::Default &&
               m_eventState == EventState::None) {
        Renderer::setCursorFunc(CursorType::Default);
        Renderer::setCurrentCursor(CursorType::Default);
    }

    if (event.button == MouseBtn::LMB || event.button == MouseBtn::RMB ||
        event.button == MouseBtn::Wheel) {
        ButtonEvent ev{
            .pressed = event.action == Action::Press, .button = event.button, .pos = event.pos
        };
        if (ev.pressed) {
            m_eventState = EventState::Pressed;
            pressedSignal.emit(ev);
        } else if (m_eventState == EventState::Pressed) {
            m_eventState = contains(event.pos) ? EventState::Hovered : EventState::None;
            releasedSignal.emit(ev);
        }
    }

    if (prevState != m_eventState)
        Renderer::markDirty();
}