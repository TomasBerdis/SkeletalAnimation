#version 450 core

in vec2 vTexCoords;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;

out vec4 color;

void main()
{
    color = texture(colorTexture, vTexCoords);
}