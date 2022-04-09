#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec4 tangent;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;
uniform vec3 uLightPos;

out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec2 vTexCoords;
out vec3 vFragPos;
out vec3 vTangentLightPos;
out vec3 vTangentCameraPos;
out vec3 vTangentFragPos;

void main()
{
    vFragPos = vec3(uModelMatrix * vec4(position, 1.0f));
    vTexCoords = texCoords;
    vNormal = normal;
    
    vec3 T = normalize(uNormalMatrix * tangent.xyz);
    vec3 N = normalize(uNormalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * tangent.w;

    mat3 TBN = transpose(mat3(T, B, N));
    vTangentLightPos    = TBN * uLightPos;
    vTangentCameraPos   = TBN * uCameraPos;
    vTangentFragPos     = TBN * vFragPos;

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(position, 1.0f);
}