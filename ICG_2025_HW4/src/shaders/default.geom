#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 vTexCoord[];

out vec2 TexCoord;

void main() {    
    for(int i = 0; i < 3; i++) {

        gl_Position = gl_in[i].gl_Position;
        
        TexCoord = vTexCoord[i];

        EmitVertex();
    }
    
    EndPrimitive();
}