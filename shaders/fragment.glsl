#version 150 core

in vec2 TexCoords;
in vec2 StandardTexCoords;

uniform sampler2DArray textureArray;
uniform sampler2D detail;

uniform int layer;
uniform float detailMix;

out vec4 outColor;

void main()
{
    vec4 overview = vec4(texture(textureArray, vec3(TexCoords.xy, layer)).rrr, 1.0);
    vec4 detail = vec4(texture(detail, StandardTexCoords).rrr, 1.0);
    outColor = mix(overview, detail, detailMix);
    //outColor = vec4(texture(textureArray, vec3(TexCoords.xy, layer)));
}