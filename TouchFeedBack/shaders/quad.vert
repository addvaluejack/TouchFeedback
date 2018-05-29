#version 330
layout (location=0) in vec2 position;
out vec2 ourTexCoord;
void main()
{
	gl_Position=vec4(position,0,1.0);
	ourTexCoord=(position+1.0)/2.0;
}
