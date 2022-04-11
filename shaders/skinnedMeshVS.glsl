#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec4 tangent;
layout(location = 4) in ivec4 boneIds;
layout(location = 5) in vec4 boneWeights;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;
uniform vec3 uLightPos;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

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

    // mat4 skinMat =
    //     boneWeights.x * finalBoneMatrices[boneIds.x] +
    //     boneWeights.y * finalBoneMatrices[boneIds.y] +
    //     boneWeights.z * finalBoneMatrices[boneIds.z] +
    //     boneWeights.w * finalBoneMatrices[boneIds.w];
    mat4 skinMat = mat4(1.0f);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (boneIds[i] < 0) 
            continue;
        skinMat += finalBoneMatrices[boneIds[i]] * boneWeights[i];
    }
    vec4 totalPosition = skinMat * vec4(position, 1.0f);
    
    vec3 T = normalize(uNormalMatrix * tangent.xyz);
    vec3 N = normalize(uNormalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * tangent.w;

    mat3 TBN = transpose(mat3(T, B, N));
    vTangentLightPos    = TBN * uLightPos;
    vTangentCameraPos   = TBN * uCameraPos;
    vTangentFragPos     = TBN * vFragPos;

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * totalPosition;
}