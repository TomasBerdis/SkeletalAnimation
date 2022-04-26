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

const int MAX_BONES = 100;  //TODO: Fix by SSBO
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

out vec3 vNormal;
out vec3 vTangent;
out vec2 vTexCoords;
out vec3 vFragPos;
out vec3 vTangentLightPos;
out vec3 vTangentCameraPos;
out vec3 vTangentFragPos;
/*DEBUG*/ out float debugWeight;

void main()
{
    mat4 skinMat =
        boneWeights.x * finalBoneMatrices[boneIds.x] +
        boneWeights.y * finalBoneMatrices[boneIds.y] +
        boneWeights.z * finalBoneMatrices[boneIds.z] +
        boneWeights.w * finalBoneMatrices[boneIds.w];
    vec4 totalPosition = skinMat * vec4(position, 1.0f);

    //DEBUG
    // debugWeight = 0.0f;
    // if (boneWeights.x > 0.01 && boneIds.x == 0)
    //     debugWeight = boneWeights.x;
    // if (boneWeights.y > 0.01 && boneIds.y == 0)
    //     debugWeight = boneWeights.y;
    // if (boneWeights.z > 0.01 && boneIds.z == 0)
    //     debugWeight = boneWeights.z;
    // if (boneWeights.w > 0.01 && boneIds.w == 0)
    //     debugWeight = boneWeights.w;
    
    vFragPos = vec3(uModelMatrix * totalPosition);
    vTexCoords = texCoords;
    vNormal = vec3(skinMat * vec4(normal, 1.0f));
    vec3 _tangent = vec3(skinMat * vec4(tangent.xyz, 1.0f));
    
    vec3 T = normalize(uNormalMatrix * _tangent);
    vec3 N = normalize(uNormalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * tangent.w;

    mat3 TBN = transpose(mat3(T, B, N));
    vTangentLightPos    = TBN * uLightPos;
    vTangentCameraPos   = TBN * uCameraPos;
    vTangentFragPos     = TBN * vFragPos;

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * totalPosition;
}