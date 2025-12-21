#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
    vec3 normal;
    vec3 fragPos;
} gs_in[];

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform float explosionProgress; // 0.0 到 1.0 控制爆炸進度

vec3 GetNormal() {
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal, float progress) {
    // 使用 smoothstep 讓爆炸效果更平滑
    float magnitude = smoothstep(0.0, 1.0, progress) * 10000.0;
    vec3 direction = normal * magnitude;
    return position + vec4(direction, 0.0);
}

void main() {
    vec3 normal = GetNormal();

    for(int i = 0; i < 3; i++) {
        gl_Position = explode(gl_in[i].gl_Position, normal, explosionProgress);
        TexCoord = gs_in[i].texCoords;
        Normal = gs_in[i].normal;
        FragPos = gs_in[i].fragPos;
        EmitVertex();
    }
    EndPrimitive();
}
