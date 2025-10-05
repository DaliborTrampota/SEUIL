#version 330 core

out vec4 FragColor;
in vec3 pos;
in vec2 uv;
in vec4 color;
in float roundness;
flat in uint type;

uniform sampler2DArray texArray;
uniform float time;

vec3 sunDir = normalize(vec3(0.2, 1, 0.2));

void main()
{
	vec4 col = texture(texArray, vec3(uv, texID));
    vec3 colData = col.xyz * max(0.5, dot(normal, sunDir));
    //FragColor = vec4(uv.x, 0, uv.y, 1.0);//col;
    //FragColor = vec4(lightCol, 1.0);
    FragColor = vec4(colData, col.a);
}