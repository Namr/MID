#version 150 core

in vec2 position;
in vec2 texCoords;
in vec2 standardTexCoords;

out vec2 TexCoords;
out vec2 StandardTexCoords;

void main()
{
    TexCoords = texCoords;
    StandardTexCoords = standardTexCoords;
    
    gl_Position = vec4(position, 0.0, 1.0);
}