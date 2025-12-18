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
    float discret_diff; // 卡通風格: 離散化亮度變化
    if (diff > 0.95)
        discret_diff = 1.0;
    else if (diff > 0.5)
        discret_diff = 0.7;
    else if (diff > 0.25)
        discret_diff = 0.4;
    else
        discret_diff = 0.1;
    vec3 diffuse = light.diffuse * material.diffuse * discret_diff * textureColor;

    vec3 R = reflect(-L, N); // binarize 高光，像是漫畫用一個圓圈代表高光那樣
    float spec = pow(max(dot(V, R), 0.0), material.gloss);
    if(spec > 0.9)
        spec = 1.0;
    else 
        spec = 0.0;
    vec3 specular = light.specular * material.specular * spec;
        
    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}