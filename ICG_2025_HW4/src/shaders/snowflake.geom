#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

in VS_OUT {
    float size;
    float rotation;
} gs_in[];

out vec3 SnowColor;
out vec2 LocalPos;

uniform mat4 view;
uniform mat4 projection;

mat2 rotate2D(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat2(c, -s, s, c);
}

void emitTriangle(vec3 center, vec2 p1, vec2 p2, vec2 p3, float rotation, vec3 color) {
    mat2 rot = rotate2D(rotation);
    
    vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 up = vec3(view[0][1], view[1][1], view[2][1]);
    
    vec2 rotP1 = rot * p1;
    vec3 pos1 = center + right * rotP1.x + up * rotP1.y;
    gl_Position = projection * view * vec4(pos1, 1.0);
    SnowColor = color;
    LocalPos = p1;
    EmitVertex();
    
    vec2 rotP2 = rot * p2;
    vec3 pos2 = center + right * rotP2.x + up * rotP2.y;
    gl_Position = projection * view * vec4(pos2, 1.0);
    SnowColor = color;
    LocalPos = p2;
    EmitVertex();
    
    vec2 rotP3 = rot * p3;
    vec3 pos3 = center + right * rotP3.x + up * rotP3.y;
    gl_Position = projection * view * vec4(pos3, 1.0);
    SnowColor = color;
    LocalPos = p3;
    EmitVertex();
    
    EndPrimitive();
}

void main() {
    vec3 center = gl_in[0].gl_Position.xyz;
    float size = gs_in[0].size;
    float rotation = gs_in[0].rotation;
    
    vec3 snowColor = vec3(0.95, 0.97, 1.0);

    float innerRadius = size * 0.3;
    float outerRadius = size;
    
    for (int i = 0; i < 6; i++) {
        float angle = float(i) * 3.14159265 / 3.0;
        float nextAngle = float(i + 1) * 3.14159265 / 3.0;
        
        vec2 p1 = vec2(0.0, 0.0);
        vec2 p2 = vec2(cos(angle) * innerRadius, sin(angle) * innerRadius);
        vec2 p3 = vec2(cos(nextAngle) * innerRadius, sin(nextAngle) * innerRadius);
        emitTriangle(center, p1, p2, p3, rotation, snowColor);
        
        vec2 branchTip = vec2(cos(angle) * outerRadius, sin(angle) * outerRadius);
        vec2 branchLeft = vec2(cos(angle - 0.15) * innerRadius, sin(angle - 0.15) * innerRadius);
        vec2 branchRight = vec2(cos(angle + 0.15) * innerRadius, sin(angle + 0.15) * innerRadius);
        emitTriangle(center, branchLeft, branchTip, branchRight, rotation, snowColor);
    }
}

