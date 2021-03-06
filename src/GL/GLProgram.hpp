#pragma once

#include "glew.h"
#include "SDL.h"
#include "glm.hpp"
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>

class GLProgram
{
public:
	GLProgram(std::vector<std::string> shaders);
	~GLProgram();
	GLuint id();
	void use();


	template <typename T>
	inline void setUniform(const GLchar* uniformName, T uniformValue)
	{
		GLint uniformLocation = glGetUniformLocation(program, uniformName);
		std::string typeName = typeid(T).name();

		if (typeName == "float")
			glProgramUniform1f(program, uniformLocation, static_cast <GLfloat> (uniformValue));
		else if (typeName == "double")
			glProgramUniform1d(program, uniformLocation, static_cast <GLdouble> (uniformValue));
		else if (typeName == "unsigned int")
			glProgramUniform1ui(program, uniformLocation, static_cast <GLuint> (uniformValue));
		else if (typeName == "int")
			glProgramUniform1i(program, uniformLocation, static_cast <GLint> (uniformValue));
	}
	template<typename T>
	void setUniformv(const GLchar* uniformName, T uniformValue);
	void setUniform(const GLchar* uniformName, bool uniformValue);
	void setUniform(const GLchar* uniformName, glm::mat3 uniformValue);
	void setUniform(const GLchar* uniformName, glm::mat4 uniformValue);
	void setUniform(const GLchar* uniformName, glm::vec2 uniformValue);
	void setUniform(const GLchar* uniformName, glm::vec3 uniformValue);
	void setUniform(const GLchar* uniformName, glm::vec4 uniformValue);
	void setUniform(const GLchar* uniformName, glm::ivec2 uniformValue);
	void setUniform(const GLchar* uniformName, glm::ivec3 uniformValue);
	void setUniform(const GLchar* uniformName, glm::ivec4 uniformValue);

	void setMatrixArrayUniform(const GLchar* uniformName, std::vector<glm::mat4>* vec);


private:
	GLuint program;

	void prepareShader(GLuint* shader, GLuint shaderType, const std::string path);
};
