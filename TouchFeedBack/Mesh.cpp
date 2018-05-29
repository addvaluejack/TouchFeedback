#include "Mesh.h"
#include <cstdlib>

using namespace std;
using namespace glm;


Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}

void Mesh::loadObjFile(const char *fileName)
{
	char *python_home;
	size_t entryCount = 0;
	_dupenv_s(&python_home, &entryCount, "PYTHONPATH");
	if (!entryCount)
	{
		_putenv("PYTHONPATH=\
C:\\Users\\game\\AppData\\Local\\Programs\\Python\\Python36;\
C:\\Users\\game\\AppData\\Local\\Programs\\Python\\Python36\\Lib;\
C:\\Users\\game\\AppData\\Local\\Programs\\Python\\Python36\\Lib\\site-packages;\
");
	}
	_dupenv_s(&python_home, &entryCount, "PYTHONPATH");
	Py_Initialize();
	if (!Py_IsInitialized())
	{
		return;
	}
	PyObject* pModule = PyImport_ImportModule("scripts.loadObj");
	if (!pModule) {
		return;
	}

	PyObject *pDict = PyModule_GetDict(pModule);
	if (!pDict)
	{
		return;
	}
	PyObject* FuncLoadObjFile = PyDict_GetItemString(pDict, "loadObjFile");

	PyObject* vertexAttributes = PyObject_CallFunction(FuncLoadObjFile, "s", fileName);

	int verticeSize=PyList_Size(vertexAttributes);
	vertices.resize(verticeSize, { {0,0,0},{0,0,0},{0,0} });
	normalEnabled = false;
	texCoordEnabled = false;
	for (int i = 0; i < verticeSize; ++i)
	{
		auto vetex=PyList_GetItem(vertexAttributes, i);
		auto position = PyList_GetItem(vetex, 0);
		auto normal = PyList_GetItem(vetex, 1);
		auto texCoord = PyList_GetItem(vetex, 2);

		VertexAttribute &v = vertices[i];
		if (PyObject_IsTrue(position))
		{
			v.position.x = PyFloat_AsDouble(PyList_GetItem(position, 0));
			v.position.y = PyFloat_AsDouble(PyList_GetItem(position, 1));
			v.position.z = PyFloat_AsDouble(PyList_GetItem(position, 2));
		}
		if (PyObject_IsTrue(normal))
		{
			normalEnabled = true;
			v.normal.x = PyFloat_AsDouble(PyList_GetItem(normal, 0));
			v.normal.y = PyFloat_AsDouble(PyList_GetItem(normal, 1));
			v.normal.z = PyFloat_AsDouble(PyList_GetItem(normal, 2));
		}
		if (PyObject_IsTrue(texCoord))
		{
			texCoordEnabled = true;
			v.texCoord.x = PyFloat_AsDouble(PyList_GetItem(texCoord, 0));
			v.texCoord.y = PyFloat_AsDouble(PyList_GetItem(texCoord, 1));
		}
	}
	Py_Finalize();


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)offsetof(VertexAttribute, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)offsetof(VertexAttribute, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)offsetof(VertexAttribute, texCoord));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::drawMesh()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);

}

