#version 150 core

in vec2 position;
in vec2 texCoords;

uniform float zoom;
uniform float xOffset;
uniform float yOffset;

out vec2 TexCoords;

void main()
{
    TexCoords = texCoords / zoom;
    TexCoords.x += xOffset;
    TexCoords.y += yOffset;
    gl_Position = vec4(position, 0.0, 1.0);
}