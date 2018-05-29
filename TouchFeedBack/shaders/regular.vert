#version 330
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texCoord;

out vec3 ourNormal;
out vec2 ourTexCoord;
out vec3 fragPos;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position=projection*view*model*vec4(position,1.0);
	ourNormal=inverse(transpose(mat3(model)))*normal;
	ourTexCoord=position.xz/10+0.5;
	fragPos=vec3(model*vec4(position,1.0));
}
