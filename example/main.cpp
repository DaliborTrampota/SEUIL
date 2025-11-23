#include <UI/Renderer.h>
#include <UI/elements/Button.h>
#include <UI/elements/Image.h>
#include <UI/elements/Label.h>
#include <UI/elements/Panel.h>


#include <memory>

using namespace ui;

int main() {
    Renderer renderer(RendererType::OpenGL, {1000, 1000});

    std::shared_ptr<Panel> panel = std::make_shared<Panel>(
        Pos<Abs, Abs>{0, 0}, Size<Rel, Rel>{1, 1}, Style<Panel>{}, AnchorPoint::None
    );


    std::shared_ptr<Panel> child = std::make_shared<Panel>(
        Pos<Rel, Rel>{0.5, 0.5}, Size<Rel, Rel>{0.5, 0.5}, Style<Panel>{}, AnchorPoint::Top
    );


    std::shared_ptr<Panel> childStyled = std::make_shared<Panel>(
        Pos<Abs, Rel>{0.f, 0.0f},
        Size<Rel, Rel>{0.5f, 0.5f},
        Style<Panel>{
            .backgroundColor = {0.f, 1.0f, .0f, 0.8f},
            .roundRadius = 50,
            .borderThickness = 30,
            .borderColor = {1.f, 0.f, 1.f}
        }
    );


    auto makeAnchorPanel = [&](AnchorPoint pt) {
        std::shared_ptr<Panel> anchorPanel = std::make_shared<Panel>(
            Pos<Abs, Abs>{0, 0},
            Size<Abs, Abs>{50, 50},
            //Size<Rel, Rel>{0.5f, 0.5f},
            Style<Panel>{
                .backgroundColor = {1.f, 0.f, 1.f, 0.8f},
                .roundRadius = 0,
                .borderThickness = 10,
                .borderColor = {1.f, 1.f, 1.f}
            },
            pt
        );

        child->addChild(anchorPanel);
    };

    makeAnchorPanel(AnchorPoint::TopLeft);
    makeAnchorPanel(AnchorPoint::TopRight);
    makeAnchorPanel(AnchorPoint::BottomLeft);
    makeAnchorPanel(AnchorPoint::BottomRight);
    makeAnchorPanel(AnchorPoint::Mid);
    makeAnchorPanel(AnchorPoint::Top);
    makeAnchorPanel(AnchorPoint::Bottom);
    makeAnchorPanel(AnchorPoint::Left);
    makeAnchorPanel(AnchorPoint::Right);

    std::shared_ptr<Image> image = std::make_shared<Image>(
        "resources/ui/test.png",
        Pos<Abs, Abs>{100, 100},
        Size<Abs, Abs>{500, 500},
        Style<Image>{.opacity = 0.8f, .pixelated = true},
        AnchorPoint::BottomLeft
        // Style<Image>{
        //     .backgroundColor = {1.f, 1.f, 1.f, 0.8f},
        // },
    );

    using BtnStyle = Style<Button>;

    BtnStyle::State normal{.background{1.f, 0.f, 0.f, 0.8f}, .border{1.f, 1.f, 1.f}};
    BtnStyle::State hover{.background{1.f, 0.f, 1.f, 0.8f}, .border{0.f, 1.f, 1.f}};
    BtnStyle::State pressed{.background{0.f, 1.f, 1.f, 0.8f}, .border{0.f, 1.f, 0.f}};


    auto btnStyle = BtnStyle{
        .normal = normal,
        .hovered = hover,
        .pressed = pressed,
        .roundRadius = 20,
        .borderThickness = 2,
    };

    std::shared_ptr<Button> button = std::make_shared<Button>(
        Pos<Abs, Abs>{600, 100}, Size<Abs, Abs>{200, 100}, btnStyle, AnchorPoint::BottomLeft
    );

    std::shared_ptr<Label> text = std::make_shared<Label>(
        "Hello world!",
        Pos<Rel, Rel>{0.f, 0.f},
        Style<Label>{
            .color = {1.f, 0.2f, 0.f, 1.f}, .font = "resources/fonts/ARIAL.TTF", .fontSize = 100
        },
        AnchorPoint::Mid,
        AnchorPoint::Top
    );


    childStyled->addChild(button);
    childStyled->addChild(image);
    panel->addChild(text);

    panel->addChild(child);
    panel->addChild(childStyled);

    renderer.setRoot(panel);

    return 0;
}