#version 330 core
layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aNormal;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 ObjPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float squeezeFactor;

void main()
{
    // TODO: Implement squeeze effect
	//   1. Adjust the vertex position to create a squeeze effect based on squeezeFactor.
	//   2. Set gl_Position to be the multiplication of the perspective matrix (projection),
	//		view matrix (view), model matrix (model) and the adjusted vertex position.
	//   3. Set TexCoord to aTexCoord.
	// Note: Ensure to handle the squeeze effect for both x and z coordinates.

    vec3 pos = aPos;
    float squeezeCofficient = sin(squeezeFactor) / 2.0;
    pos.x = aPos.x + aPos.z * squeezeCofficient;
    pos.z = aPos.z + aPos.x * squeezeCofficient;
	
	gl_Position = projection * view * model * vec4(pos, 1.0);
    
	TexCoord = aTexCoord;
    ObjPos = aPos;

}