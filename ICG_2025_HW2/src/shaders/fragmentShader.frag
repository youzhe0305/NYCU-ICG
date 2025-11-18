#version 330 core
out vec4 FragColor;

in vec2 TexCoord; 
in vec3 ObjPos;

uniform sampler2D ourTexture;
uniform vec3 breathingColor;
uniform float intensity;
uniform float stripeFrequency;
uniform int useStripes;

void main()
{
    // TODO: Implement Breathing Light Effect
	//   1. Retrieve the color from the texture at texCoord.
	//   2. Set FragColor to be the texture color * (breathingColor * intensity)
	//   Note: Ensure FragColor is appropriately set for both breathing light and normal cases.
		
	vec4 textureColor = texture(ourTexture, TexCoord);

	float stripeSwitch = 1.0;
	if (useStripes == 1) {
		const float PI = 3.14159265359;
		float angle = atan(ObjPos.y, ObjPos.x);
		float norm = (angle + PI) / (2.0 * PI);
		float t = norm * stripeFrequency;
		stripeSwitch = step(0.5, fract(t));
	}

	vec3 finalRgb = textureColor.rgb * stripeSwitch * (breathingColor * intensity);
	FragColor = vec4(finalRgb, textureColor.a);
} 