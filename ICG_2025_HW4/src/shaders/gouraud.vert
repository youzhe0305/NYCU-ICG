#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// TODO:
// Implement Gouraud shading

out vec3 ColorCoeff;
out vec2 TexCoord;

struct light_t {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct material_t {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float gloss;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform light_t light;
uniform material_t material;
uniform vec3 viewPos;

void main()
{
    vec3 WorldPos = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    
    vec3 N = normalize(Normal);
    vec3 L = normalize(light.position - WorldPos);
    vec3 V = normalize(viewPos - WorldPos);

    // caculate color coefficient of texture color in vertex shader => intrpolate in fragment shader and apply to texture color
    vec3 ambient = light.ambient * material.ambient;

    float diff = max(dot(L, N), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff;

    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), material.gloss);
    vec3 specular = light.specular * material.specular * spec;  

    ColorCoeff = ambient + diffuse + specular;
    TexCoord = aTexCoord;
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}