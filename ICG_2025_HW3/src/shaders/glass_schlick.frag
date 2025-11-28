#version 330 core

out vec4 FragColor;

in vec3 WorldPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
    vec3 I = normalize(WorldPos - viewPos);
    vec3 N = normalize(Normal);
    float n1 = 1.0; // AIR_coeff
    float n2 = 1.52; // GLASS_coeff
    float eta = n1 / n2;

    vec3 R = reflect(I, N);

    vec3 T = refract(I, N, eta);
    float R0 = pow((n1 - n2) / (n1 + n2), 2.0);
    vec3 V = -I; // I dot N => negative => compute with -I
    float cosTheta = max(dot(N, V), 0.0);
    float Rtheta = R0 + (1.0 - R0) * pow(1.0 - cosTheta, 5.0);

    vec4 reflectColor = texture(skybox, R);
    vec4 refractColor = texture(skybox, T);

    FragColor = mix(refractColor, reflectColor, Rtheta);

}
