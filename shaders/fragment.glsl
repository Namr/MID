#version 150 core

in vec2 TexCoords;
in vec2 StandardTexCoords;

uniform sampler3D textureArray;
uniform sampler2D detail;

uniform int view;
uniform float layer;
uniform float detailMix;

out vec4 outColor;

void main()
{
    vec4 overview = vec4(texture(textureArray, vec3(TexCoords.x, layer, TexCoords.y)).rrr, 1.0);

    if(view == 0)
        overview = vec4(texture(textureArray, vec3(TexCoords.x, TexCoords.y, layer)).rrr, 1.0);
    if(view == 1)
        overview = vec4(texture(textureArray, vec3(layer, TexCoords.x, TexCoords.y)).rrr, 1.0);
    
    vec4 detail = vec4(texture(detail, StandardTexCoords).rrr, 1.0);
    outColor = mix(overview, detail, detailMix) * vec4(1.7, 1.7, 1.7, 1.0);
    //outColor = vec4(texture(textureArray, vec3(TexCoords.xy, layer)));
}