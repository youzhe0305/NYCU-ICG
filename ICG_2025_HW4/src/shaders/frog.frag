#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D frogTexture;
uniform vec3 viewPos;
uniform float time;

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
    vec4 texColor = texture(frogTexture, TexCoords);
    vec3 baseColor = texColor.rgb;
    
    if (texColor.a < 0.1) {
        baseColor = vec3(0.2, 0.6, 0.3);
    }
    
    vec3 norm = normalize(Normal);
    
    vec3 ambient = light.ambient * material.ambient * 0.7;
    
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff * 1.2;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.gloss);
    vec3 specular = light.specular * material.specular * spec * 0.4;
    
    vec3 result = (ambient + diffuse + specular) * baseColor;

    result *= 1.3;
    
    FragColor = vec4(result, 1.0);
}
