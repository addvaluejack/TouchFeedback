#include "TouchForce.h"
#include <vector>
#include <algorithm>
#include <numeric>

using namespace glm;
using namespace std;

TouchForce::TouchForce()
{
}


TouchForce::~TouchForce()
{
}

void TouchForce::loadResource()
{
	shaderProgram.LoadShadersFromFile("shaders/force.vert", "shaders/force.frag");
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*10, 0,GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TouchForce::drawForce(glm::mat4 VPMatrix)
{
	shaderProgram.use();
	shaderProgram.setUniform("VP", VPMatrix);
	glBindVertexArray(VAO);
	for (auto force:forces)
	{
		shaderProgram.setUniform("forceColor", force.color);
		vector<vec3> data;
		//force origin
		data.push_back(force.origin);
		//force tail
		vec3 tail = force.origin + force.force;
		data.push_back(tail);
		//arrow
		float ratio = 0.2;
		vec3 tangent = normalize(cross({0,0,1}, force.force));
		vec3 bitangent= normalize(cross(tangent, force.force));

		data.push_back(tail);
		data.push_back(tail + (tangent -normalize(force.force))*ratio);

		data.push_back(tail);
		data.push_back(tail - (tangent + normalize(force.force))*ratio);

		data.push_back(tail);
		data.push_back(tail + (bitangent - normalize(force.force))*ratio);

		data.push_back(tail);
		data.push_back(tail - (bitangent + normalize(force.force))*ratio);

		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0,sizeof(vec3)*data.size(), &data[0]);

		glDrawArrays(GL_LINES, 0, data.size());
	}
	glBindVertexArray(VAO);
	shaderProgram.done();
}

void TouchForce::caculateForceOriginal(Mesh &mesh, glm::mat4 modelMatrix)
{
	forces.clear();

	float maxforce = -1;
	FeedBackForce f{ { 0, 0, 0 }, { 0,10,0 }, { 1,0,0 } };

	vector<vec3> transformedVertice;
	for (int i = 0; i < mesh.vertices.size(); ++i)
	{
		transformedVertice.push_back(vec3(modelMatrix*vec4(mesh.vertices[i].position, 1.0f)));
	}

	for (int i = 0; i < transformedVertice.size(); ++i)
	{
		vec3 &pos =transformedVertice[i];
		if (pos.y < 0)	//candidate
		{
			vec3 normal = normalize(cross(transformedVertice[(i / 3)*3+1] - transformedVertice[(i / 3)*3+0], transformedVertice[(i / 3)*3+2] - transformedVertice[(i / 3)*3+0]));
			if (normal.y>-0.5)
			{
				continue;
			}
			vec3 force = -normal / normal.y*pos.y;
			if (length(force)>maxforce)
			{
				maxforce = length(force);
				f.origin = { pos.x,0,pos.z };
				f.force = force*10.0f;
			}
		}
	}
	if (maxforce>0)
	{
		f.force.x = 0;
		f.force.z = 0;
		forces.push_back(f);
	}
}

void TouchForce::caculateForceEnhanced(Mesh &mesh, glm::mat4 modelMatrix)
{
	forces.clear();
	FeedBackForce f{ { 0, 0, 0 },{ 0,10,0 },{ 1,0,0 } };

	vector<vec3> points;

	vector<vec3> transformedVertice;
	for (int i = 0; i < mesh.vertices.size(); ++i)
	{
		transformedVertice.push_back(vec3(modelMatrix*vec4(mesh.vertices[i].position, 1.0f)));
	}

	for (int i = 0; i < transformedVertice.size(); ++i)
	{
		vec3& pos = transformedVertice[i];
		if (pos.y < 0)	//candidate
		{
			vec3 normal = normalize(cross(transformedVertice[(i / 3) * 3 + 1] - transformedVertice[(i / 3) * 3 + 0], transformedVertice[(i / 3) * 3 + 2] - transformedVertice[(i / 3) * 3 + 0]));
			float len = length(normal);
			if (normal.y > -0.5 || _isnan(len))
			{
				continue;
			}

			if (find_if(points.begin(), points.end(), [&pos](auto &testp) {return length(testp - pos) < 0.0001;}) == points.end())
			{
				vec3 force = -normal / normal.y*pos.y;
				f.origin = { pos.x,0,pos.z };
				f.force = force*10.0f;
				f.force.x = 0;
				f.force.z = 0;
				forces.push_back(f);
				points.push_back(pos);
			}

		}
	}

	if (points.size())
	{
		vec3 sumPos;
		for (auto p : points)
		{
			sumPos += p;
		}
		vec3 sumF;
		for (auto &f:forces)
		{
			sumF += f.force;
		}
		f.origin = { sumPos.x / float(points.size()),0,sumPos.z / float(points.size()) };
		f.force = sumF;
		f.color = { 0,1,0 };
		f.force.x = 0;
		f.force.z = 0;
		forces.push_back(f);
	}
}
