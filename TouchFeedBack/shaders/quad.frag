#version 330
in vec2 ourTexCoord;
uniform sampler2D quadTex;
out vec4 color;
void main()
{
	color=texture(quadTex,ourTexCoord);
}
