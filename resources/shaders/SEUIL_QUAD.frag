#version 450 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec4 FragColor;

uniform float time;


in vec2 uv;
flat in vec4 color;
// flat in uint type;
flat in uint borderRoundness;
flat in uint borderThickness;
flat in vec4 borderColor;


bool perfectBorderRadius = true;
// clang-format off
{{Border.glsl}}
// clang-format on


void main() {
    FragColor = color;
    drawBorder(borderRoundness, borderThickness, borderColor);
}