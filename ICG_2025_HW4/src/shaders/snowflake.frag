#version 330 core
out vec4 FragColor;

in vec3 SnowColor;
in vec2 LocalPos;

void main()
{
    float alpha = 0.85;
    
    float dist = length(LocalPos);
    alpha *= smoothstep(1.0, 0.0, dist * 0.3);
    alpha = clamp(alpha, 0.6, 0.9);
    
    FragColor = vec4(SnowColor, alpha);
}
