#include <UI/Panel.h>
#include <UI/Renderer.h>

using namespace ui;


void Panel::addChild(std::shared_ptr<UIElement> child) {
    m_children.push_back(std::move(child));
}

bool Panel::contains(const glm::vec2& point) const {
    if (m_parent == nullptr) {}
    return false;
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

// TODO maybe change to std::visit compile time dispatch?
glm::ivec2 Panel::calculateSizeAndPosition(const glm::ivec2& parentSize) {
    glm::ivec2 calculated{};
    if (std::holds_alternative<Size<Rel, Abs>>(m_size)) {
        auto& size = std::get<Size<Rel, Abs>>(m_size);
        size.calcX = parentSize.x * size.x;
        size.calcY = size.y;

        calculated = glm::ivec2(size.calcX, size.calcY);
    } else if (std::holds_alternative<Size<Abs, Rel>>(m_size)) {
        auto& size = std::get<Size<Abs, Rel>>(m_size);
        size.calcX = size.x;
        size.calcY = parentSize.y * size.y;

        calculated = glm::ivec2(size.calcX, size.calcY);
    } else if (std::holds_alternative<Size<Rel, Rel>>(m_size)) {
        auto& size = std::get<Size<Rel, Rel>>(m_size);
        size.calcX = parentSize.x * size.x;
        size.calcY = parentSize.y * size.y;

        calculated = glm::ivec2(size.calcX, size.calcY);
    } else if (std::holds_alternative<Size<Auto, Auto>>(m_size)) {
        assert(false && "Auto size is not supported");
    } else if (std::holds_alternative<Size<Abs, Abs>>(m_size)) {
        auto& size = std::get<Size<Abs, Abs>>(m_size);
        size.calcX = size.x;
        size.calcY = size.y;

        calculated = glm::ivec2(size.calcX, size.calcY);
    }

    return calculated;
}

void Panel::visit(Renderer& renderer) {
    renderer.renderPanel(*this);
}