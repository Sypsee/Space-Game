#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

out vec3 normal;
out vec3 fragPos;

void main()
{
    normal = normalize(aPos);
    fragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = proj * view * vec4(fragPos, 1.0);
}