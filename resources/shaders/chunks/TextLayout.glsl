
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in uint aColorIndex;  // index into SSBO or for image the alpha

out vec2 uv;
out vec4 color;

void setVarying() {
    uv = aUV;
    color = colorPalette[aColorIndex];
}
