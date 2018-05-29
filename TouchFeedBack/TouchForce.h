#pragma once
#include "Mesh.h"
#include "shader.h"
#include "Camera.h"


struct FeedBackForce 
{
	glm::vec3 origin;
	glm::vec3 force;
	glm::vec3 color;
};

class TouchForce
{
	Shader shaderProgram;
	std::vector<FeedBackForce> forces;
	GLuint VAO = 0;
	GLuint VBO = 0;
public:
	TouchForce();
	~TouchForce();
	void loadResource();
	void drawForce(glm::mat4 VPMatrix);

	void caculateForceOriginal(Mesh &mesh, glm::mat4 modelMatrix);
	void caculateForceEnhanced(Mesh &mesh, glm::mat4 modelMatrix);
};

