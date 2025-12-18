#version 330 core

out vec4 FragColor;

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoord;

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

uniform light_t light;
uniform material_t material;
uniform vec3 viewPos;
uniform sampler2D objectTexture;

void main()
{

    vec3 textureColor = texture(objectTexture, TexCoord).rgb;
    vec3 N = normalize(Normal);
    vec3 L = normalize(light.position - WorldPos);
    vec3 V = normalize(viewPos - WorldPos);

    vec3 ambient = light.ambient * material.ambient * textureColor;

    float diff = max(dot(L, N), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff * textureColor;

    // Halfway Vector H = (L + V) / ||L + V||
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), material.gloss);
    vec3 specular = light.specular * material.specular * spec;  
        
    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}