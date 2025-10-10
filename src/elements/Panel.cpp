#include <UI/Renderer.h>
#include <UI/elements/Panel.h>


using namespace ui;


void Panel::addChild(std::shared_ptr<UIElement> child) {
    m_children.push_back(std::move(child));
}

void Panel::mouseMove(const glm::ivec2& pos) {
    if (contains(pos)) {
        if (m_eventState == EventState::None) {
            // trigger enter
            if (m_hoverEvent) {
                // trigger hover event
            }
            m_eventState = EventState::Hovered;
        } else if (m_hoverEvent) {
            // trigger hover event
        }
    } else {
        if (m_eventState == EventState::Hovered) {
            // trigger leave event
            m_eventState = EventState::None;
        }
    }
}

void Panel::visit(Renderer& renderer) {
    renderer.renderPanel(*this);
}