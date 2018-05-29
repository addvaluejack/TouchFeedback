#pragma once
#include "gl/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <stdio.h>

class Shader
{
	GLuint programID;
	Shader(const Shader &) = delete;
	Shader(Shader && lhs) = delete;
	Shader & operator = (const Shader &) = delete;
	Shader & operator = (Shader && lhs) = delete;

public:
	Shader();
	~Shader();
	void use();

	void done();

	//从字符串读取shader
	bool LoadShadersFromString(const char * vertexShaderSource, const char * fragmentShaderSource, const char * geometryShaderSource=0);

	//从文件读取shader
	bool LoadShadersFromFile(const char * vertex_file_path, const char * fragment_file_path, const char * geometry_file_path=0);

	template<typename T>
	void setUniform(const char *variableName, T var);

	template<>
	void setUniform(const char *variableName, float var)
	{
		int loc = glGetUniformLocation(programID, variableName);
		if (loc < 0)
		{
			printf("uniform variable %s doesn't exist!\n", variableName);
		}
		else
		{
			glUniform1f(loc, var);
		}
	}

	template<>
	void setUniform(const char *variableName, glm::vec2 var)
	{
		int loc = glGetUniformLocation(programID, variableName);
		if (loc < 0)
		{
			printf("uniform variable %s doesn't exist!\n", variableName);
		}
		else
		{
			glUniform2fv(loc, 1, value_ptr(var));
		}
	}

	template<>
	void setUniform(const char *variableName, glm::vec3 var)
	{
		int loc = glGetUniformLocation(programID, variableName);
		if (loc < 0)
		{
			printf("uniform variable %s doesn't exist!\n", variableName);
		}
		else
		{
			glUniform3fv(loc, 1, value_ptr(var));
		}
	}

	template<>
	void setUniform(const char *variableName, int var)
	{
		int loc = glGetUniformLocation(programID, variableName);
		if (loc < 0)
		{
			printf("uniform variable %s doesn't exist!\n", variableName);
			int err = glGetError();
			printf("error%d\n", err);
		}
		else
		{
			glUniform1i(loc, var);
		}
	}

	template<>
	void setUniform(const char *variableName, unsigned int var)
	{
		int loc = glGetUniformLocation(programID, variableName);
		if (loc < 0)
		{
			printf("uniform variable %s doesn't exist!\n", variableName);
		}
		else
		{
			glUniform1i(loc, var);
		}
	}


	template<>
	void setUniform(const char *variableName, glm::mat4 var)
	{
		int loc = glGetUniformLocation(programID, variableName);
		if (loc < 0)
		{
			printf("uniform variable %s doesn't exist!\n", variableName);
		}
		else
		{
			glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(var));
		}
	}

	template<>
	void setUniform(const char *variableName, glm::mat3 var)
	{
		int loc = glGetUniformLocation(programID, variableName);
		if (loc < 0)
		{
			printf("uniform variable %s doesn't exist!\n", variableName);
		}
		else
		{
			glUniformMatrix3fv(loc, 1, GL_FALSE, value_ptr(var));
		}
	}

	template<>
	void setUniform(const char *variableName, bool var)
	{
		int loc = glGetUniformLocation(programID, variableName);
		if (loc < 0)
		{
			printf("uniform variable %s doesn't exist!\n", variableName);
		}
		else
		{
			glUniform1i(loc, var);
		}
	}

};

