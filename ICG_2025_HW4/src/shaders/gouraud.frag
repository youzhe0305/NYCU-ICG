#version 330 core

out vec4 FragColor;
// TODO:
// Implement Gouraud shading
    
in vec3 ColorCoeff;
in vec2 TexCoord;

uniform sampler2D objectTexture;

void main()
{
    vec4 texColor = texture(objectTexture, TexCoord);
    FragColor = vec4(ColorCoeff * texColor.rgb, 1.0);
}