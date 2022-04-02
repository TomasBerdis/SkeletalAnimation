#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 vNormal;
out vec2 vTexCoords;
out vec3 vFragPos;

void main()
{
    vTexCoords = texCoords;
    vNormal = normal;
    vFragPos = vec3(uModelMatrix * vec4(position, 1.0f));
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(position, 1.0f);
}