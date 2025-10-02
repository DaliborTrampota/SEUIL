#include <UI/Panel.h>

int main() {
    ui::Panel panel(
        ui::Pos<ui::Abs, ui::Abs>{0, 0},
        ui::Size<ui::Rel, ui::Rel>{1, 1},
        ui::Style<ui::Panel>{},
        ui::AnchorPoint::None
    );


    ui::Panel child(
        ui::Pos<ui::Rel, ui::Rel>{0.5, 0.5},
        ui::Size<ui::Rel, ui::Rel>{0.5, 0.5},
        ui::Style<ui::Panel>{},
        ui::AnchorPoint::MidTop
    );

    panel.addChild(std::move(child));

    return 0;
}