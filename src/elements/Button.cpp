#include <UI/elements/Button.h>

#include <UI/Renderer.h>

using namespace ui;

bool Button::isPressed() const {
    return m_eventState == EventState::Pressed;
}

bool Button::isHovered() const {
    return m_eventState == EventState::Hovered;
}

void Button::visit(Renderer& renderer) {
    renderer.renderButton(*this);
}