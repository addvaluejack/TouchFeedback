#pragma once
#include <vector>
#include "gl/glew.h"
#include "glm/glm.hpp"
#include "pythonDefs.h"

struct VertexAttribute
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};


class Mesh
{
	friend class TouchForce;
	std::vector<VertexAttribute> vertices;
	bool normalEnabled = false;
	bool texCoordEnabled = false;

	GLuint VAO = 0;
	GLuint VBO = 0;

public:
	Mesh();
	~Mesh();
	void loadObjFile(const char *fileName);
	void drawMesh();
};

