#include <UI/Renderer.h>
#include <UI/elements/Panel.h>


using namespace ui;


void Panel::addChild(std::shared_ptr<UIElement> child) {
    m_children.push_back(std::move(child));
}

void Panel::mouseEvent(const MouseEvent& event) {
    UIElement::mouseEvent(event);

    for (auto& child : m_children) {
        EventState chState = child->state();
        if (child->contains(event.pos)) {
            child->mouseEvent(event);
        } else if ((event.action == Action::Move && chState != EventState::None) ||
                   (event.action == Action::Release && chState == EventState::Pressed)) {
            // Hovering panel but not child -> Trigger hover stop and exit
            child->mouseEvent(event);
        }
    }
}

void Panel::visit(Renderer& renderer) {
    renderer.renderPanel(*this);
}