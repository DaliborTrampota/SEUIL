#version 450 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec4 FragColor;
layout(std430, binding = 0) readonly buffer TextureHandles {
    sampler2D images[];
};
uniform float time;

in vec2 uv;
// flat in uint type;
flat in vec4 color;
flat in uint borderRoundness;
// flat in uint borderThickness;
// flat in vec4 borderColor;
flat in uint imageIndex;

bool perfectBorderRadius = true;
// clang-format off
{{Border.glsl}}
// clang-format on


void main() {
    vec4 texColor = texture(images[imageIndex], uv);
    FragColor = vec4(texColor.rgb, texColor.a * color.a);
    drawBorder(borderRoundness, 0, vec4(0.0, 0.0, 0.0, 1.0));
}