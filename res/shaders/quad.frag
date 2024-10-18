#version 460 core

out vec4 fragColor;

uniform sampler2D screenTex;
uniform vec2 screenExtent;

in vec2 uv;

const float gamma = 2.0;
const float numColors = 30.0;
const float pixelSize = 0.035;

void main()
{
    float pixelSizeScaled = pixelSize * 100;
    vec2 pixelatedCoords = vec2(floor(uv.x * screenExtent.x / pixelSizeScaled) * pixelSizeScaled / screenExtent.x,
                                floor(uv.y * screenExtent.y / pixelSizeScaled) * pixelSizeScaled / screenExtent.y);
    vec3 c = textureLod(screenTex, pixelatedCoords, 0).rgb;
    
    c = pow(c, gamma.rrr);
    c = c * numColors;
    c = floor(c);
    c = c / numColors;
    c = pow(c, vec3(1.0/gamma));

    fragColor = vec4(c, 1.0);
}
