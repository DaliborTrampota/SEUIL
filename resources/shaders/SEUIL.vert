#version 450 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec4 aColor;
layout (location = 3) in uint aRoundness;
layout (location = 4) in uint aType;

layout (location = 5) in uint aBorderThickness;
layout (location = 6) in vec3 aBorderColor;
layout (location = 7) in uint aData;

uniform vec2 uScreenSize;

out vec3 pos;
out vec2 uv;
out vec4 color;
flat out uint roundness;
flat out uint type;
flat out uint borderThickness;
out vec4 borderColor;
flat out uint data;

void main()
{
    type = aType;
    uv = aUV;
    pos = aPos;
    color = aColor;
    roundness = aRoundness;
    borderThickness = aBorderThickness;
    borderColor = vec4(aBorderColor, color.a);
    data = aData;
    
    // Convert from [0, width] to [-1, 1]
    vec2 ndc = vec2(
        (pos.x / uScreenSize.x) * 2.0 - 1.0,
        1.0 - (pos.y / uScreenSize.y) * 2.0
    );

    gl_Position = vec4(ndc, 0.0, 1.0);
}