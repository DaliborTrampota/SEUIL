
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in uint aColorIndex;
layout(location = 3) in uvec3 aBorder;  // roundness, border thickness, border color index
// layout(location = 3) in uint aType;

out vec2 uv;
flat out vec4 color;

// flat out uint type;
flat out uint borderRoundness;
flat out uint borderThickness;
flat out vec4 borderColor;


void setVarying() {
    uv = aUV;
    color = colorPalette[aColorIndex];
    // type = aType;
    borderRoundness = aBorder.x;
    borderThickness = aBorder.y;
    borderColor = vec4(colorPalette[aBorder.z].rgb, color.a);
}