#version 450 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in uint aColorIndex;  // index into SSBO or for image the alpha value
layout(location = 3) in uint aType;
layout(location = 4) in uvec3 aBorder;  // roundness, border thickness, border color index

// Image: the texture index for image in frag shader
// Button: the button state (0: normal, 1: hovered, 2: pressed)
layout(location = 5) in uint aData;
layout(location = 6) in float aFloatData;

// layout(location = 7) in uvec3 aButtonColorIndices;

layout(std430, binding = 1) readonly buffer Colors {
    vec4 colorPalette[];
};

uniform vec2 uScreenSize;

out vec2 uv;
flat out uint type;
flat out uint roundness;
flat out uint borderThickness;

flat out vec4 color;
flat out vec4 borderColor;
// flat out vec4 hoverColor;
// flat out vec4 pressedColor;

flat out uint imageIndex;
flat out uint data;
flat out float floatData;

void main() {
    type = aType;
    uv = aUV;
    roundness = aBorder.x;
    borderThickness = aBorder.y;
    data = aData;
    floatData = aFloatData;

    if (type == uint(1) || type == uint(4)) {  // Image or ImagePanel
        imageIndex = aColorIndex;
        color = vec4(1.0, 1.0, 1.0, aFloatData);
    } else {
        color = colorPalette[aColorIndex];
    }

    borderColor = vec4(colorPalette[aBorder.z].rgb, color.a);
    // hoverColor = colorPalette[aButtonColorIndices.x];
    // pressedColor = colorPalette[aButtonColorIndices.y];


    // Convert from [0, width] to [-1, 1]
    // NDC has Y inverted, meaning increasing Y goes down on the screen
    vec2 ndc = vec2((aPos.x / uScreenSize.x) * 2.0 - 1.0, 1.0 - (aPos.y / uScreenSize.y) * 2.0);

    gl_Position = vec4(ndc, 0.0, 1.0);
}