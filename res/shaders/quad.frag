#version 460 core

out vec4 fragColor;

#define STEP_COUNT 5
#define FLT_MAX 10000

uniform sampler2D screenTex;
uniform sampler2D depthTex;
uniform vec2 screenExtent;
uniform vec3 camPos;
uniform mat4 inverseProj;
uniform mat4 inverseView;

in vec3 fragPos;
in vec2 uv;

const float gamma = 2.0;
const float numColors = 30.0;
const float pixelSize = 0.035;

vec2 raySphereIntersection(vec3 rayOrigin, vec3 rayDirection, vec3 pos, float radius)
{
    float a = dot(rayDirection, rayDirection);
    vec3 s0_r0 = rayOrigin - pos;
    float b = 2.0 * dot(rayDirection, s0_r0);
    float c = dot(s0_r0, s0_r0) - (radius * radius);
    float d = b*b - 4.0*a*c;
    if (d > 0.0)
    {
        float s = sqrt(d);
        float t0 = max((-b - s)/(2.0*a), 0.0);
        float t1 = (-b + s)/(2.0*a);

        if (t1 >= 0)
        {
            return vec2(t0, t1 - t0);
        }
    }

    return vec2(FLT_MAX, 0);
}

void main()
{
    float pixelSizeScaled = pixelSize * 100;
    vec2 pixelatedCoords = vec2(floor(uv.x * screenExtent.x / pixelSizeScaled) * pixelSizeScaled / screenExtent.x,
                                floor(uv.y * screenExtent.y / pixelSizeScaled) * pixelSizeScaled / screenExtent.y);
    vec3 c = textureLod(screenTex, pixelatedCoords, 0).rgb;

    float nonLinearDepth = texture(depthTex, uv).r;
    float depth = 0.1 / nonLinearDepth;
    // depth = clamp(depth, 0.0, 1.0);
    
    c = pow(c, gamma.rrr);
    c = c * numColors;
    c = floor(c);
    c = c / numColors;
    c = pow(c, vec3(1.0/gamma));

    vec4 target = inverseProj * vec4(fragPos.x, fragPos.y, 1, 1);
	vec3 rayDir = vec3(inverseView * vec4(normalize(vec3(target) / target.w), 0));
    vec2 hitPoints = raySphereIntersection(camPos, rayDir, vec3(0), 120);

    // c = vec3(hitPoints.y / 240);

    fragColor = vec4(vec3(depth), 1.0);
}