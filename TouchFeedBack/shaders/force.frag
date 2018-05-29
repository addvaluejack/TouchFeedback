#version 330
out vec4 color;

uniform vec3 forceColor;

void main()
{
	color=vec4(forceColor,1);
}
