
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in uint aImageIndex;
layout(location = 3) in float aOpacity;
layout(location = 4) in uint aRoundRadius;


out vec2 uv;
// flat out uint type;
flat out vec4 color;
flat out uint borderRoundness;
// flat out uint borderThickness;
// flat out vec4 borderColor;
flat out uint imageIndex;

void setVarying() {
    uv = aUV;
    // type = aType;
    color = vec4(1.0, 1.0, 1.0, aOpacity);
    borderRoundness = aRoundRadius;
    imageIndex = aImageIndex;
}