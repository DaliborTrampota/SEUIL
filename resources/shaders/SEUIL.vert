#version 450 core
#extension GL_ARB_bindless_texture : require

layout(std430, binding = 1) readonly buffer Colors {
    vec4 colorPalette[];
};

// clang-format off
{{vertex_layout}}
// clang-format on

uniform vec2 uScreenSize;


void main() {
    setVarying();
    // Convert from [0, width] to [-1, 1]
    // NDC has Y inverted, meaning increasing Y goes down on the screen
    vec2 ndc = vec2((aPos.x / uScreenSize.x) * 2.0 - 1.0, 1.0 - (aPos.y / uScreenSize.y) * 2.0);

    gl_Position = vec4(ndc, 0.0, 1.0);
}