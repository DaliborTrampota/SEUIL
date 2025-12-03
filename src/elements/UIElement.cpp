#include <UI/elements/UIElement.h>

#include <UI/Events.h>

using namespace ui;

bool UIElement::contains(const glm::vec2& point) const {
    return point.x >= m_calculatedDims.x && point.x <= m_calculatedDims.x + m_calculatedDims.z &&
           point.y >= m_calculatedDims.y && point.y <= m_calculatedDims.y + m_calculatedDims.w;
}

glm::ivec4 UIElement::calculateSizeAndPosition(const glm::ivec4& parentPosSize) {
    // make temp ivec4 here instead of capturing this:

    m_calculatedDims = {0, 0, 0, 0};

    std::visit(
        [this, &parentPosSize](auto& size) {
            using T = std::decay_t<decltype(size)>;
            if constexpr (std::is_same_v<T, Size<Rel, Abs>>) {
                m_calculatedDims.z = parentPosSize.z * size.x;
                m_calculatedDims.w = size.y;
            } else if constexpr (std::is_same_v<T, Size<Abs, Rel>>) {
                m_calculatedDims.z = size.x;
                m_calculatedDims.w = parentPosSize.w * size.y;
            } else if constexpr (std::is_same_v<T, Size<Abs, Abs>>) {
                m_calculatedDims.z = size.x;
                m_calculatedDims.w = size.y;
            } else if constexpr (std::is_same_v<T, Size<Rel, Rel>>) {
                m_calculatedDims.z = parentPosSize.z * size.x;
                m_calculatedDims.w = parentPosSize.w * size.y;
            } else if constexpr (std::is_same_v<T, Size<Auto, Auto>>) {
                //assert(false && "Auto size is not supported");
            }
        },
        m_size
    );

    // TODO anchor point
    std::visit(
        [this, &parentPosSize](auto& pos) {
            glm::ivec2 modifier = adjustAnchorPoint(m_anchorPoint, m_calculatedDims, parentPosSize);

            using T = std::decay_t<decltype(pos)>;
            if constexpr (std::is_same_v<T, Pos<Rel, Abs>>) {
                m_calculatedDims.x += parentPosSize.z * pos.x * modifier.x + parentPosSize.x;
                m_calculatedDims.y += pos.y * modifier.y + parentPosSize.y;
            } else if constexpr (std::is_same_v<T, Pos<Abs, Rel>>) {
                m_calculatedDims.x += pos.x * modifier.x + parentPosSize.x;
                m_calculatedDims.y += parentPosSize.w * pos.y * modifier.y + parentPosSize.y;
            } else if constexpr (std::is_same_v<T, Pos<Rel, Rel>>) {
                m_calculatedDims.x += parentPosSize.z * pos.x * modifier.x + parentPosSize.x;
                m_calculatedDims.y += parentPosSize.w * pos.y * modifier.y + parentPosSize.y;
            } else if constexpr (std::is_same_v<T, Pos<Abs, Abs>>) {
                m_calculatedDims.x += pos.x * modifier.x + parentPosSize.x;
                m_calculatedDims.y += pos.y * modifier.y + parentPosSize.y;
            }
        },
        m_position
    );

    return m_calculatedDims;
}


glm::ivec2 UIElement::adjustAnchorPoint(
    AnchorPoint anchor, glm::ivec4& posSize, const glm::ivec4& parentPosSize
) const {
    if (anchor == AnchorPoint::None)
        return {1.f, 1.f};

    bool top = static_cast<uint8_t>(anchor & AnchorPoint::Top);
    bool bottom = static_cast<uint8_t>(anchor & AnchorPoint::Bottom);
    bool left = static_cast<uint8_t>(anchor & AnchorPoint::Left);
    bool right = static_cast<uint8_t>(anchor & AnchorPoint::Right);
    // no need to check for mid, because it will be handled by the x and y checks

    bool x = left || right;
    bool y = top || bottom;

    // shift in the middle of the parent and by half of the element so it is centered (same for !y)
    if (!x)
        posSize.x = parentPosSize.z / 2 - posSize.z / 2;

    if (!y)
        posSize.y = parentPosSize.w / 2 - posSize.w / 2;

    if (top)
        posSize.y = 0;

    if (bottom)
        posSize.y = parentPosSize.w - posSize.w;

    if (left)
        posSize.x = 0;

    if (right)
        posSize.x = parentPosSize.z - posSize.z;

    return {right ? -1 : 1, bottom ? -1 : 1};
}

void UIElement::mouseEvent(const MouseEvent& event) {
    if (event.action == Action::Move) {
        if (contains(event.pos)) {
            HoverEvent ev{.hoverStart = true, .pos = event.pos};
            if (m_eventState == EventState::None) {
                m_eventState = EventState::Hovered;
                enterSignal.emit(ev);
                hoverSignal.emit(ev);
            } else {
                ev.hoverStart = false;
                hoverSignal.emit(ev);
            }
        } else {
            if (m_eventState == EventState::Hovered) {
                m_eventState = EventState::None;
                HoverEvent ev{.hoverStart = false, .pos = event.pos};
                exitSignal.emit(ev);
            }
        }
    }
}