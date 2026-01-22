#version 450 core

out vec4 FragColor;
in vec2 uv;
in vec4 color;

uniform sampler2D uMSDF;
uniform float pxRange;  // set to distance field's pixel range

float screenPxRange() {
    vec2 unitRange = vec2(pxRange) / vec2(textureSize(uMSDF, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(uv);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

vec4 bgColor = vec4(0.0, 0.0, 0.0, 0.0);

void main() {
    vec3 msd = texture(uMSDF, uv).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    FragColor = mix(bgColor, color, opacity);
}