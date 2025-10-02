#include <UI/UIElement.h>

using namespace ui;


glm::ivec4 UIElement::calculateSizeAndPosition(const glm::ivec4& parentPosSize) {
    glm::ivec4 calculatedDims{};

    std::visit(
        [&calculatedDims, &parentPosSize](auto& pos) {
            using T = std::decay_t<decltype(pos)>;
            if constexpr (std::is_same_v<T, Pos<Rel, Abs>>) {
                pos.calc.x = parentPosSize.x * pos.x;
                pos.calc.y = pos.y;
            } else if constexpr (std::is_same_v<T, Pos<Abs, Rel>>) {
                pos.calc.x = pos.x;
                pos.calc.y = parentPosSize.y * pos.y;
            } else if constexpr (std::is_same_v<T, Pos<Rel, Rel>>) {
                pos.calc.x = parentPosSize.x * pos.x;
                pos.calc.y = parentPosSize.y * pos.y;
            } else if constexpr (std::is_same_v<T, Pos<Abs, Abs>>) {
                pos.calc.x = pos.x;
                pos.calc.y = pos.y;
            }
            calculatedDims.x = pos.calc.x;
            calculatedDims.y = pos.calc.y;
        },
        m_position
    );

    std::visit(
        [&calculatedDims, &parentDims](auto& position) {
            using T = std::decay_t<decltype(position)>;
            if constexpr (std::is_same_v<T, Pos<Rel, Abs>>) {
                position.calc.x = parentDims.x * position.x;
                position.calc.y = position.y;
            } else if constexpr (std::is_same_v<T, Pos<Abs, Rel>>) {
                position.calc.x = position.x;
                position.calc.y = parentDims.y * position.y + parentDims.w;
            } else if constexpr (std::is_same_v<T, Pos<Rel, Abs>>) {
                position.calc.x = parentDims.x * position.x + parentDims.z;
                position.calc.y = position.y;
            } else if constexpr (std::is_same_v<T, Pos<Rel, Rel>>) {
                position.calc.x = parentDims.x * position.x + parentDims.z;
                position.calc.y = parentDims.y * position.y + parentDims.w;
            }
            calculatedDims.x = position.calc.x;
            calculatedDims.y = position.calc.y;
        },
        m_position
    );

    return calculatedDims;
}

bool UIElement::contains(const glm::vec2& point) const {
    glm::ivec2 size = std::get<Size<Abs, Abs>>(m_size).calc;
    glm::ivec2 position = std::get<Pos<Abs, Abs>>(m_position).calc;

    return point.x >= position.x && point.x <= position.x + size.x && point.y >= position.y &&
           point.y <= position.y + size.y;
}
