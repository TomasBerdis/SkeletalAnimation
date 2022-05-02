#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec4 tangent;
layout(location = 4) in ivec4 boneIds;
layout(location = 5) in vec4 boneWeights;
layout(std430, binding = 6) buffer matrixLayout
{
    mat4 finalBoneMatrices[];
};

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;
uniform vec3 uLightPos;

out vec3 vNormal;
out vec2 vTexCoords;
out vec3 vTangentLightPos;
out vec3 vTangentCameraPos;
out vec3 vTangentFragPos;

void main()
{
    mat4 skinMat =
        boneWeights.x * finalBoneMatrices[boneIds.x] +
        boneWeights.y * finalBoneMatrices[boneIds.y] +
        boneWeights.z * finalBoneMatrices[boneIds.z] +
        boneWeights.w * finalBoneMatrices[boneIds.w];

    vec4 totalPosition = skinMat * vec4(position, 1.0f);
    
    vec3 T   = normalize(uNormalMatrix * tangent.xyz);
    vec3 N   = normalize(uNormalMatrix * normal);
    T        = normalize(T - dot(T, N) * N); // Gram-Schmidt process
    vec3 B   = cross(N, T) * tangent.w;
    mat3 TBN = transpose(mat3(skinMat) * mat3(T, B, N));
    
    vTangentLightPos    = TBN * uLightPos;
    vTangentCameraPos   = TBN * uCameraPos;
    vTangentFragPos     = TBN * vec3(uModelMatrix * totalPosition);
    vNormal             = vec3(skinMat * vec4(normal, 0.0f));
    vTexCoords          = texCoords;

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * totalPosition;
}