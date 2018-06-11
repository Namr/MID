#version 150 core

in vec2 TexCoords;

uniform sampler2DArray textureArray;
uniform int layer;

out vec4 outColor;

void main()
{
    outColor = vec4(texture(textureArray, vec3(TexCoords.xy, layer)).rrr, 1.0);
    //outColor = vec4(texture(textureArray, vec3(TexCoords.xy, layer)));
}