# SEUIL - Simple Enough UI Library

**SEUIL** is a lightweight, OpenGL-based retained mode UI library for modern C++ applications. Designed with simplicity and flexibility in mind, it provides an intuitive API for creating responsive user interfaces with minimal boilerplate.

## Features

- **Flexible Positioning System** - Mix and match absolute and relative positioning/sizing with anchor point system
- **Event System** - Signal-based event handling for mouse interactions
- **Modern UI Elements** - Panels, Images, and Buttons with customizable styles
- **Hierarchical Layout** - Parent-child relationships with automatic layout calculation
- **GPU-Accelerated Rendering** - Efficient OpenGL-based renderer with bindless textures
- **Rounded Corners & Borders** - Built-in support for modern UI aesthetics
- **CMake Integration** - Easy to integrate into existing projects

## Dependencies

- FreeType for text
- stb_image.h

## Requirements

- C++20 compatible compiler
- GLAD for OpenGL 4.5+
- GLM (OpenGL Mathematics)
- CMake 3.15+

## Quick Start

### Installation

Add SEUIL as a subdirectory in your CMake project:

```cmake
add_subdirectory(dependencies/seuil)
target_link_libraries(your_target PRIVATE seuil)
```

Copy resources to your build directory:

```cmake
setup_seuil_resource_copy(your_target)
```

### Basic Usage

Before calling `Renderer::update` it is important that cursor creation function is registered to `Renderer` via `Renderer::registerCursorFunction`

Such implementation can look like this with GLFW

```cpp
Renderer::registerCursorFunction([](CursorType type) {
    switch (type) {
        case CursorType::Pointer:
            if (s_currentCursor) {
                glfwDestroyCursor(s_currentCursor);
                s_currentCursor = nullptr;
            }
            s_currentCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
            glfwSetCursor(glfwGetCurrentContext(), s_currentCursor);
            break;
        case CursorType::Default:
            if (s_currentCursor) {
                glfwDestroyCursor(s_currentCursor);
                s_currentCursor = nullptr;
            }
            //s_currentCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
            //glfwSetCursor(glfwGetCurrentContext(), s_currentCursor);
            break;
        case CursorType::Type:
            if (s_currentCursor) {
                glfwDestroyCursor(s_currentCursor);
                s_currentCursor = nullptr;
            }
            s_currentCursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
            glfwSetCursor(glfwGetCurrentContext(), s_currentCursor);
            break;
    }
});
```

```cpp
#include <UI/Renderer.h>
#include <UI/elements/Panel.h>
#include <UI/elements/Button.h>

// Create renderer with viewport size
ui::Renderer renderer(ui::RendererType::OpenGL, {1920, 1080});

// Create root panel (full screen, relative sizing)
auto root = std::make_shared<ui::Panel>(
    ui::Pos<ui::Abs, ui::Abs>{0, 0},
    ui::Size<ui::Rel, ui::Rel>{1.0f, 1.0f},
    ui::Style<ui::Panel>{}
);

// Create a centered child panel
auto panel = std::make_shared<ui::Panel>(
    ui::Pos<ui::Rel, ui::Rel>{0.5f, 0.5f},
    ui::Size<ui::Abs, ui::Abs>{400, 300},
    ui::Style<ui::Panel>{
        .backgroundColor = {0.2f, 0.2f, 0.2f, 0.9f}
    },
    ui::AnchorPoint::Mid  // Center anchor point
);

// Add button with event handler
auto button = std::make_shared<ui::Button>(
    ui::Pos<ui::Rel, ui::Rel>{0.5f, 0.8f},
    ui::Size<ui::Abs, ui::Abs>{200, 50},
    ui::Style<ui::Button>{
        .backgroundColor = {0.3f, 0.5f, 0.8f, 1.0f},
        .roundRadius = 5,
        .hoveredColor = {0.4f, 0.6f, 0.9f, 1.0f},
        .pressedColor = {0.2f, 0.4f, 0.7f, 1.0f}
    },
    ui::AnchorPoint::Mid
);

// Connect button event
button->pressedSignal.connect([](const ui::MouseEvent& e) {
    std::cout << "Button clicked!" << std::endl;
});

// Build hierarchy
panel->addChild(button);
root->addChild(panel);
renderer.setRoot(root);

// In your render loop:
renderer.update(deltaTime);

// Handle mouse events:
// You have to call mouseEvent on Renderer whenever mouse is moved or pressed
ui::MouseEvent event{
    .type = ui::MouseEvent::LMB,
    .pos = {mouseX, mouseY}
};
renderer.mouseEvent(event);
```

## Core Concepts

### Positioning and Sizing

SEUIL uses a flexible type-safe positioning system that allows mixing absolute (pixels) and relative (normalized 0-1) coordinates:

```cpp
// Position types
ui::Pos<ui::Abs, ui::Abs>{100, 200}    // 100px from left, 200px from top
ui::Pos<ui::Rel, ui::Rel>{0.5f, 0.5f}  // 50% from left, 50% from top
ui::Pos<ui::Abs, ui::Rel>{100, 0.5f}   // 100px from left, 50% from top

// Size types
ui::Size<ui::Abs, ui::Abs>{400, 300}   // 400x300 pixels
ui::Size<ui::Rel, ui::Rel>{0.8f, 0.6f} // 80% width, 60% height
ui::Size<ui::Auto, ui::Auto>{}          // Auto-size (future feature)
```

### Anchor Points

Anchor points determine how elements are aligned relative to their position:

```cpp
ui::AnchorPoint::None         // Top-left corner (default)
ui::AnchorPoint::Mid          // Center
ui::AnchorPoint::Top          // Top center
ui::AnchorPoint::Bottom       // Bottom center
ui::AnchorPoint::Left         // Middle left
ui::AnchorPoint::Right        // Middle right
ui::AnchorPoint::TopLeft      // Top-left corner
ui::AnchorPoint::TopRight     // Top-right corner
ui::AnchorPoint::BottomLeft   // Bottom-left corner
ui::AnchorPoint::BottomRight  // Bottom-right corner
```

### Event System

SEUIL uses a signal-slot pattern for event handling:

```cpp
// Available signals
element->hoverSignal.connect([](const ui::MouseEvent& e) { /* ... */ });
button->pressedSignal.connect([](const ui::ButtonEvent& e) { /* ... */ });

// Mouse event types
ui::MouseEvent::Move       // Mouse movement
ui::MouseEvent::LMB        // Left mouse button
ui::MouseEvent::RMB        // Right mouse button
ui::MouseEvent::MMB        // Middle mouse button
ui::MouseEvent::WheelUp    // Scroll up
ui::MouseEvent::WheelDown  // Scroll down
```

## UI Elements

### Panel

Container element that can hold child elements:

```cpp
ui::Style<ui::Panel> style{
    .backgroundColor = {r, g, b, a},  // RGBA color
    .roundRadius = 10,                 // Corner radius in pixels
    .borderThickness = 2,              // Border thickness in pixels
    .borderColor = {r, g, b}          // RGB color (alpha from backgroundColor)
};
```

### Image

Displays textures:

```cpp
ui::Style<ui::Image> style{
    .roundRadius = 5,      // Corner radius
    .opacity = 1.0f,       // 0.0 to 1.0
    .pixelated = false     // Use nearest-neighbor filtering
};

// Load image
unsigned int imageId = ui::Renderer::imageDataMgr.loadImage("path/to/image.png");
image->setImageId(imageId);
```

### Button

Interactive button element:

```cpp
ui::Style<ui::Button> style{
    .backgroundColor = {0.3f, 0.5f, 0.8f, 1.0f},
    .roundRadius = 5,
    .borderThickness = 1,
    .borderColor = {1.0f, 1.0f, 1.0f},
    .textColor = {1.0f, 1.0f, 1.0f},
    .textHoverColor = {1.0f, 1.0f, 0.0f},
    .pressedColor = {0.2f, 0.4f, 0.7f, 1.0f},
    .hoveredColor = {0.4f, 0.6f, 0.9f, 1.0f}
};

button->pressedSignal.connect([](const ui::MouseEvent& e) {
    // Handle button press
});
```

## Building

### As Part of a Project

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Example Application

An example application is provided in the `example/` directory:

```bash
cd example
mkdir build && cd build
cmake ..
cmake --build .
./seuil_example
```

## Architecture

- **Renderer** - Main interface for UI rendering and event management
- **RendererImpl** - Backend-specific implementation (currently OpenGL)
- **UIElement** - Base class for all UI elements with layout system
- **Signal** - Observer pattern for event handling
- **ImageDataMgr** - Centralized texture/image management

## Roadmap

- [ ] Text rendering support with FreeType integration
- [ ] Additional UI elements (Checkbox, Slider, TextField)
- [ ] Layout containers (HBox, VBox, Grid)
- [ ] Animation system
- [ ] Theming support?
- [ ] Vulkan backend?

## Contributing

Contributions are welcome! Feel free to submit issues and pull requests.

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

Copyright 2025 SEUIL Contributors
