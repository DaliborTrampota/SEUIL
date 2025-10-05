#include <UI/OpenGLRendererImpl.h>
#include <UI/Panel.h>
#include <UI/Renderer.h>

#include <memory>

int main() {
    ui::Renderer renderer(ui::RendererType::OpenGL, {1000, 1000});

    std::shared_ptr<ui::Panel> panel = std::make_shared<ui::Panel>(
        ui::Pos<ui::Abs, ui::Abs>{0, 0},
        ui::Size<ui::Rel, ui::Rel>{1, 1},
        ui::Style<ui::Panel>{},
        ui::AnchorPoint::None
    );


    std::shared_ptr<ui::Panel> child = std::make_shared<ui::Panel>(
        ui::Pos<ui::Rel, ui::Rel>{0.5, 0.5},
        ui::Size<ui::Rel, ui::Rel>{0.5, 0.5},
        ui::Style<ui::Panel>{},
        ui::AnchorPoint::MidTop
    );

    panel->addChild(std::move(child));

    renderer.setRoot(panel);

    return 0;
}