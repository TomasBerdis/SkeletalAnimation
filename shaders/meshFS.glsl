#version 450 core

in vec3 vNormal;
in vec2 vTexCoords;
in vec3 vFragPos;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform vec3 uCameraPos;
uniform vec3 uLightPos;
uniform vec3 uLightColor;

out vec4 color;

void main()
{
    vec4 tSample = texture(colorTexture, vTexCoords);

    // Ambient
    float ambientStrenght = 0.1f;
    vec3 ambient = ambientStrenght * uLightColor;

    // Diffuse
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vFragPos);

    float delta = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = delta * uLightColor;

    // Specular
    float specularStrength = 0.5f;
    vec3 cameraDir = normalize(uCameraPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;  

    // Phong
    vec3 result = (ambient + diffuse + specular) * vec3(tSample.xyz);
    color = vec4(result, 1.0f);
}