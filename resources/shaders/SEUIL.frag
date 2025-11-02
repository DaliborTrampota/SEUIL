#version 450 core
#extension GL_ARB_bindless_texture : require

layout(std430, binding = 0) readonly buffer TextureHandles {
    sampler2D images[];
};

out vec4 FragColor;

in vec2 uv;
flat in uint type;
flat in uint roundness;
flat in uint borderThickness;

in vec4 color;
in vec4 borderColor;
in vec4 hoverColor;
in vec4 pressedColor;

flat in uint data;


bool perfectBorderRadius = true;

uniform float time;

void renderRoundedCorners(uint r1, uint r2, vec2 panelSize, vec2 uvPixelPos, vec4 color) {
    if (perfectBorderRadius)
        r2 = r1 - borderThickness;

    vec2 TL = vec2(0, panelSize.y);
    vec2 TR = vec2(panelSize.x, panelSize.y);
    vec2 BL = vec2(0, 0);
    vec2 BR = vec2(panelSize.x, 0);

    vec2 cornerTL = vec2(1, -1);
    vec2 cornerTR = vec2(-1, -1);
    vec2 cornerBL = vec2(1, 1);
    vec2 cornerBR = vec2(-1, 1);
    
    if (uvPixelPos.x < r1 && uvPixelPos.y > panelSize.y - r1) {
        if (distance(uvPixelPos, TL + cornerTL * r1) > r2) 
            FragColor = color;
        if (distance(uvPixelPos, TL + cornerTL * r1) > r1)
            discard;
    } 

    if (uvPixelPos.x > panelSize.x - r1 && uvPixelPos.y > panelSize.y - r1) {
        if (distance(uvPixelPos, TR + cornerTR * r1) > r2)
            FragColor = color;
        if (distance(uvPixelPos, TR + cornerTR * r1) > r1)
            discard;
    }
    if (uvPixelPos.x < r1 && uvPixelPos.y < r1) {
        if (distance(uvPixelPos, BL + cornerBL * r1) > r2)
            FragColor = color;
        if (distance(uvPixelPos, BL + cornerBL * r1) > r1)
            discard;
    }

    if (uvPixelPos.x > panelSize.x - r1 && uvPixelPos.y < r1) {
        if (distance(uvPixelPos, BR + cornerBR * r1) > r2)
            FragColor = color;
        if (distance(uvPixelPos, BR + cornerBR * r1) > r1)
            discard;
    }
}

void main()
{

    if (type == uint(1)) { // Image
        FragColor = vec4(texture(images[0], uv).rgb, color.a);
    } else {
        FragColor = color;
    }
    
    if (roundness > 0.0f) {
        // fwidth gives us the rate of change per screen pixel
        vec2 uvDeriv = fwidth(uv);
        // Estimate panel dimensions: if UV goes from 0 to 1, then 1.0 / derivative = dimension
        vec2 panelSize = 1.0 / uvDeriv;
        // local pixel space (0 to panelSize)
        vec2 localPos = uv * panelSize;
        
        renderRoundedCorners(roundness, borderThickness, panelSize, localPos, borderColor);
    }

    if (borderThickness > uint(0)) {
        vec2 uvDeriv = fwidth(uv);
        vec2 panelSize = 1.0 / uvDeriv;
        vec2 localPos = uv * panelSize;

        if (localPos.x < borderThickness || 
            localPos.x > panelSize.x - borderThickness || 
            localPos.y < borderThickness || 
            localPos.y > panelSize.y - borderThickness) {
            FragColor = borderColor;
        }

    }
}