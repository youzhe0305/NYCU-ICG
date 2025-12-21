#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aSize;
layout (location = 2) in float aRotation;

out VS_OUT {
    float size;
    float rotation;
} vs_out;

uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{
    vs_out.size = aSize;
    vs_out.rotation = aRotation + time * 0.5;
    
    gl_Position = vec4(aPos, 1.0);
}

