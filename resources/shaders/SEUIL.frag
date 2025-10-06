#version 330 core

out vec4 FragColor;

in vec3 pos;
in vec2 uv;
in vec4 color;
flat in float roundness;
flat in uint type;

uniform float time;

void main()
{
    FragColor = color;
    
    // discard pixels outside the rounded corners
    if (roundness > 0.0f) {
        // Calculate panel dimensions in pixels using derivatives
        // fwidth gives us the rate of change per screen pixel
        vec2 uvDeriv = fwidth(uv);
        
        // Estimate panel dimensions: if UV goes from 0 to 1, then 1.0 / derivative = dimension
        vec2 panelSize = 1.0 / uvDeriv;
        
        // local pixel space (0 to panelSize)
        vec2 localPos = uv * panelSize;
        
        // local pixel space
        vec2 cornerTL = vec2(roundness, panelSize.y - roundness);
        vec2 cornerTR = vec2(panelSize.x - roundness, panelSize.y - roundness);
        vec2 cornerBL = vec2(roundness, roundness);
        vec2 cornerBR = vec2(panelSize.x - roundness, roundness);
        
        if (localPos.x < roundness && localPos.y > panelSize.y - roundness) {
            if (distance(localPos, cornerTL) > roundness) {
                discard;
            }
        } else if (localPos.x > panelSize.x - roundness && localPos.y > panelSize.y - roundness) {
            if (distance(localPos, cornerTR) > roundness) {
                discard;
            }
        } else if (localPos.x < roundness && localPos.y < roundness) {
            if (distance(localPos, cornerBL) > roundness) {
                discard;
            }
        } else if (localPos.x > panelSize.x - roundness && localPos.y < roundness) {
            if (distance(localPos, cornerBR) > roundness) {
                discard;
            }
        }
    }
}