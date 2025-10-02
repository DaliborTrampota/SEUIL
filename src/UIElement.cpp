#include <UI/UIElement.h>

using namespace ui;


glm::ivec4 UIElement::calculateSizeAndPosition(const glm::ivec4& parentPosSize) {
    // make temp ivec4 here instead of capturing this:
    // TODO anchor point
    std::visit(
        [this, &parentPosSize](auto& pos) {
            using T = std::decay_t<decltype(pos)>;
            if constexpr (std::is_same_v<T, Pos<Rel, Abs>>) {
                m_calculatedDims.x = parentPosSize.x * pos.x + parentPosSize.x;
                m_calculatedDims.y = pos.y + parentPosSize.y;
            } else if constexpr (std::is_same_v<T, Pos<Abs, Rel>>) {
                m_calculatedDims.x = pos.x + parentPosSize.x;
                m_calculatedDims.y = parentPosSize.y * pos.y + parentPosSize.y;
            } else if constexpr (std::is_same_v<T, Pos<Rel, Rel>>) {
                m_calculatedDims.x = parentPosSize.x * pos.x + parentPosSize.x;
                m_calculatedDims.y = parentPosSize.y * pos.y + parentPosSize.y;
            } else if constexpr (std::is_same_v<T, Pos<Abs, Abs>>) {
                m_calculatedDims.x = pos.x + parentPosSize.x;
                m_calculatedDims.y = pos.y + parentPosSize.y;
            }
        },
        m_position
    );

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
                assert(false && "Auto size is not supported");
            }
        },
        m_size
    );

    return m_calculatedDims;
}

bool UIElement::contains(const glm::vec2& point) const {
    return point.x >= m_calculatedDims.x && point.x <= m_calculatedDims.x + m_calculatedDims.z &&
           point.y >= m_calculatedDims.y && point.y <= m_calculatedDims.y + m_calculatedDims.w;
}
