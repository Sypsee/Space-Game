#version 460 core

layout (location = 0) out vec4 fragColor;

#define STEP_COUNT 10
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
const float numColors = 64.0;
const float pixelSize = 0.035;
const float atmosphereScale = 0.5;
const vec3 sunDir = vec3(200,200,200);
const vec3 waveLengths = vec3(700, 530, 440);
const float scatteringStrength = 1;
const float densityFalloff = 4.3;

const float atmosphereRadius = (1 + atmosphereScale) * 100;
const float scatteringR = pow(400 / waveLengths.x, 4) * scatteringStrength;
const float scatteringG = pow(400 / waveLengths.y, 4) * scatteringStrength;
const float scatteringB = pow(400 / waveLengths.z, 4) * scatteringStrength;

const vec3 scatteringCoefficients = vec3(scatteringR, scatteringG, scatteringB);

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

float densityAtPoint(vec3 densitySamplePoint)
{
    float heightAboveSurface = length(densitySamplePoint - vec3(0)) - 100;
    float height01 = heightAboveSurface / (atmosphereRadius - 100);
    float localDensity = exp(-height01 * densityFalloff) * (1 - height01);
    return localDensity;
}

float opticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength)
{
    vec3 densitySamplePoint = rayOrigin;
    float stepSize = rayLength / (STEP_COUNT - 1);
    float opticalDepth = 0;

    for (int i = 0; i < STEP_COUNT; i++)
    {
        float localDensity = densityAtPoint(densitySamplePoint);
        opticalDepth += localDensity * stepSize;
        densitySamplePoint += rayDir * stepSize;
    }

    return opticalDepth;
}

vec3 calculateLight(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 originalColor)
{
    vec3 inScatteringPoint = rayOrigin;
    float stepSize = rayLength / (STEP_COUNT - 1);
    vec3 inScatterdLight = vec3(0);
    float viewRayOpticalDepth = 0;

    for (int i = 0; i < STEP_COUNT; i++)
    {
        float sunRayLength = raySphereIntersection(inScatteringPoint, sunDir, vec3(0), atmosphereRadius).y;
        float sunRayOpticalDepth = opticalDepth(inScatteringPoint, sunDir, sunRayLength);
        viewRayOpticalDepth = opticalDepth(inScatteringPoint, -rayDir, stepSize * i);
        vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatteringCoefficients);
        float localDensity = densityAtPoint(inScatteringPoint);

        inScatterdLight += localDensity * transmittance * scatteringCoefficients * stepSize;
        inScatteringPoint += rayDir * stepSize;
    }
    float originalColTransmittance = exp(-viewRayOpticalDepth);

    return originalColor * originalColTransmittance + inScatterdLight;
}

void main()
{
    float pixelSizeScaled = pixelSize * 100;
    vec2 pixelatedCoords = vec2(floor(uv.x * screenExtent.x / pixelSizeScaled) * pixelSizeScaled / screenExtent.x,
                                floor(uv.y * screenExtent.y / pixelSizeScaled) * pixelSizeScaled / screenExtent.y);
    vec3 c = textureLod(screenTex, pixelatedCoords, 0).rgb;

    float nonLinearDepth = texture(depthTex, uv).r;
    float depth = 0.001 / nonLinearDepth;

    vec4 target = inverseProj * vec4(fragPos.x, fragPos.y, 1, 1);
	vec3 rayDir = vec3(inverseView * vec4(normalize(vec3(target) / target.w), 0));
    vec2 hitPoints = raySphereIntersection(camPos, rayDir, vec3(0), atmosphereRadius);

    hitPoints.y = min(hitPoints.y, depth - hitPoints.x);

    if (hitPoints.y > 0)
    {
        float epsilon = 0.0001;
        vec3 pointInAtmosphere = camPos + rayDir * (hitPoints.x + epsilon);
        vec3 light = calculateLight(pointInAtmosphere, rayDir, hitPoints.y - epsilon * 2, c);
        
        c = light;
    }

    c = pow(c, gamma.rrr);
    c = c * numColors;
    c = floor(c);
    c = c / numColors;
    c = pow(c, vec3(1.0/gamma));

    fragColor = vec4(c, 1.0);
}
