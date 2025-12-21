#version 330 core
out vec4 FragColor;

in vec2 TexCoord; 
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform vec3 viewPos;
uniform float explosionProgress;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float gloss;
};

uniform Light light;
uniform Material material;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    
    // 基本光照計算
    vec3 ambient = light.ambient * material.ambient;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.gloss);
    vec3 specular = light.specular * (spec * material.specular);
    
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    
    // 爆炸時添加火焰效果
    if (explosionProgress > 0.0) {
        vec3 fireColor = mix(vec3(1.0, 0.3, 0.0), vec3(1.0, 0.8, 0.0), explosionProgress);
        result = mix(result, fireColor, explosionProgress * 0.6);
    }
    
    FragColor = vec4(result, texColor.a);
}
