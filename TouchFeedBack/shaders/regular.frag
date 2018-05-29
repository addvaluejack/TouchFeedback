#version 330
in vec3 ourNormal;
in vec2 ourTexCoord;
in vec3 fragPos;

uniform sampler2D tex;

out vec4 color;

uniform int isPlane;

uniform mat4 shadowMatrix;


void main()
{
	float diff=max(0,dot(normalize(ourNormal),normalize(vec3(1,1,1))))*0.8+0.2;
	//color=vec4(diff,diff,diff,1.0);
	color=texture(tex, ourTexCoord)*max(0,dot(normalize(ourNormal),normalize(vec3(1,1,1))));
	if(isPlane==0 && abs(fragPos.y)<0.1)
	{
		color=vec4(1,0,0,1);
	}
}
