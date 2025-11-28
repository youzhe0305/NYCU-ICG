#version 330 core
// TODO: Implement metallic shading

out vec4 FragColor;

in vec3 WorldPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{

    vec3 I = normalize(WorldPos - viewPos);
    vec3 N = normalize(Normal);
    vec3 R = reflect(I, N);
    // directly reflect the enviroment, not to consider object color
    FragColor = texture(skybox, R);

}
