#version 450 core

in vec3 vNormal;
in vec2 vTexCoords;
in vec3 vTangentLightPos;
in vec3 vTangentCameraPos;
in vec3 vTangentFragPos;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform vec3 uCameraPos;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform bool uUseNormalMap;

out vec4 color;

void main()
{
    vec3 normal;
    if (uUseNormalMap)
    {
        normal = texture(normalMap, vTexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }
    else
        normal = vNormal;

    vec3 colorSample = texture(diffuseMap, vTexCoords).xyz;

    // Ambient
    float ambientStrenght = 0.1f;
    vec3 ambient = ambientStrenght * colorSample;

    // Diffuse
    vec3 lightDir   = normalize(vTangentLightPos - vTangentFragPos);
    float delta     = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse    = delta * colorSample;

    // Specular
    float specularStrength = 0.5f;
    vec3 cameraDir  = normalize(vTangentCameraPos - vTangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + cameraDir);
    float spec      = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular   = vec3(0.2) * spec;  
    
    color = vec4(ambient + diffuse + specular, 1.0f);
}