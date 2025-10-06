#version 330 core

out vec4 FragColor;

in vec3 pos;
in vec2 uv;
in vec4 color;
in float roundness;
flat in uint type;

uniform float time;

void main()
{
    // Use the color passed from the vertex shader (includes alpha for transparency)
    FragColor = color;
}