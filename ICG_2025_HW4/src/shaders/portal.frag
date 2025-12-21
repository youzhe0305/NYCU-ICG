#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D objectTexture;
uniform float time;
uniform float progress;

// random函式
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
    vec2 uv = TexCoord;
    vec2 center = vec2(0.5, 0.5);
    vec2 toCenter = uv - center;
    float dist = length(toCenter);
    //座標 用來做： 中心較穩定 邊邊不穩定

    // portal mask
    // 0~1的權重    中心 ~ 1 邊界 ~ 0
    float radius = 0.5;
    float feather = 0.06;
    float mask = smoothstep(radius, radius - feather, dist);

    // glitch特效強度 剛開始->強 快結束->弱
    float glitchStrength = clamp(1.0 - progress, 0.0, 1.0);

    // 擾動portal 中心比較少 邊界看起來擾動大
    float phase = sin(time * 12.0 + dist * 40.0);
    vec2 microOffset = normalize(toCenter + 0.0001) * phase * 0.01 * (1.0 - dist);
    uv += microOffset * mask;

    // block glitch特效
    float blockSize = 15.0;
    vec2 block = floor(uv * blockSize);
    float noise = random(block + floor(time * 30.0));
    
    // 越來越stable
    float glitchDensity = pow(1.0 - progress, 1.2);
    bool isGlitchBlock = (noise < glitchDensity * 0.6) && (dist < 0.8);

    if (isGlitchBlock) {
        float offset = (random(block) - 0.5) * 0.08;
        uv.x += offset * mask;
    }

    // 色差
    float chroma = dist * glitchStrength * 0.12 * mask;

    float r = texture(objectTexture, uv + vec2(chroma, 0.0)).r;
    float g = texture(objectTexture, uv).g;
    float b = texture(objectTexture, uv - vec2(chroma, 0.0)).b;
    float a = texture(objectTexture, uv).a;

    if (a < 0.1)
        discard;

    // block color改變
    if (isGlitchBlock) {
        float type = random(block + vec2(1.0));
        if (type < 0.33) { // rgb錯亂
            float t = r; r = g; g = b; b = t;
        } else if (type < 0.66) { // 負片
            r = 1.0 - r;
            g = 1.0 - g;
            b = 1.0 - b;
        } else { // 變亮
            r *= 1.5;
            g *= 1.5;
            b *= 1.5;
        }
    }

    vec3 finalColor = vec3(r, g, b);

    // energy flow 
    float wave = sin(dist * 28.0 - time * 7.0);
    float energy = smoothstep(0.2, 0.8, wave) * mask;

    vec3 energyColor = mix(
        vec3(0.3, 0.6, 1.0),   // portal blue
        vec3(1.0, 0.5, 0.2),   // meteor orange
        progress
    );

    finalColor += energyColor * energy * progress * 0.8;

    // 空間裂縫
    float crack = random(vec2(dist * 25.0, time * 6.0));
    if (crack > 0.975 && progress > 0.6) {
        finalColor += vec3(2.0) * mask;
    }

    // flash
    if (progress > 0.8 && progress < 1.0) {
        float t = (progress - 0.8) / 0.2;
        float flash = sin(t * 3.14159);
        finalColor += vec3(flash * 2.0) * mask;
    }

    // edge glow
    float edge = smoothstep(0.45, 0.5, dist);
    vec3 edgeGlow = vec3(0.4, 0.7, 1.0) * edge * 2.8;

    finalColor += edgeGlow;
    FragColor = vec4(finalColor, a * mask);
}