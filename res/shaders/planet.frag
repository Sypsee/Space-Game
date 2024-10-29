#version 450 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

uniform vec3 u_color;
uniform vec2 u_minMax;
uniform bool u_isLightSource;

const vec3 lightPos = vec3(0.0, 0.0, 0.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float ambientStrength = 0.8;

vec3 getColorFromHeight(float heightFactor) {
    vec3 waterColor = vec3(0.0, 0.0, 1.0);
    vec3 sandColor = vec3(1.0, 0.9, 0.5);
    vec3 grassColor = vec3(0.0, 0.5, 0.0);
    vec3 rockColor = vec3(0.5, 0.5, 0.5);
    vec3 snowColor = vec3(1.0, 1.0, 1.0);
    vec3 flowerColor = vec3(1.0, 0.0, 0.0);
    vec3 dirtColor = vec3(0.5, 0.35, 0.2);
    vec3 forestColor = vec3(0.0, 0.3, 0.0);

    if (heightFactor < 0.2) {
        return mix(waterColor, sandColor, heightFactor * 5.0);
    } else if (heightFactor < 0.4) {
        return mix(sandColor, grassColor, (heightFactor - 0.2) * 5.0);
    } else if (heightFactor < 0.6) {
        return mix(grassColor, forestColor, (heightFactor - 0.4) * 5.0);
    } else if (heightFactor < 0.8) {
        return mix(forestColor, rockColor, (heightFactor - 0.6) * 5.0);
    } else {
        return mix(rockColor, snowColor, (heightFactor - 0.8) * 5.0);
    }
}

void main()
{
    float heightFactor = smoothstep(u_minMax.x, u_minMax.y, length(fragPos) * 0.3);
    vec3 baseColor = getColorFromHeight(heightFactor);
    // vec3 baseColor = vec3(1.0);

    vec3 finalColor = vec3(1.0);

    if (!u_isLightSource) {
        finalColor = mix(u_color, baseColor, 0.3);
        vec3 lightDir = normalize(lightPos - fragPos);
        float diff = max(dot(normalize(normal), lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        finalColor = (diffuse + vec3(ambientStrength)) * finalColor;
    }
    else
    {
        finalColor = mix(u_color, baseColor, 0.85);
    }

    fragColor = vec4(finalColor, 1.0);
}