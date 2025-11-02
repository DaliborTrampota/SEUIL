#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in uint aColorIndex; // index into SSBO or for image the alpha 

layout (std430, binding = 1) readonly buffer Colors {
    vec4 colorPalette[];
};

uniform vec2 uScreenSize;

out vec2 uv;
out vec4 color;

void main()
{
    uv = aUV;
    color = colorPalette[aColorIndex];
    
    // Convert from [0, width] to [-1, 1]
    vec2 ndc = vec2(
        (aPos.x / uScreenSize.x) * 2.0 - 1.0,
        1.0 - (aPos.y / uScreenSize.y) * 2.0
    );

    gl_Position = vec4(ndc, 0.0, 1.0);
}