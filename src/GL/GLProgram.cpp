#include "GLProgram.hpp"


GLProgram::GLProgram(std::vector<std::string> shaderPaths)
{
	GLuint vertexShader		 = -1;
	GLuint fragmentShader	 = -1;
	GLuint tessControlShader = -1;
	GLuint tessEvalShader	 = -1;
	GLuint geometryShader	 = -1;

	std::vector<GLuint *> shaders;
	shaders.push_back(&vertexShader);
	shaders.push_back(&fragmentShader);
	shaders.push_back(&tessControlShader);
	shaders.push_back(&tessEvalShader);
	shaders.push_back(&geometryShader);

	for (size_t i = 0; i < shaderPaths.size(); i++)
	{
		if (shaderPaths[i].ends_with("VS.glsl"))
			prepareShader(&vertexShader, GL_VERTEX_SHADER, shaderPaths[i]);
		else if (shaderPaths[i].ends_with("FS.glsl"))
			prepareShader(&fragmentShader, GL_FRAGMENT_SHADER, shaderPaths[i]);
		else if (shaderPaths[i].ends_with("TCS.glsl"))
			prepareShader(&tessControlShader, GL_TESS_CONTROL_SHADER, shaderPaths[i]);
		else if (shaderPaths[i].ends_with("TES.glsl"))
			prepareShader(&tessEvalShader, GL_TESS_EVALUATION_SHADER, shaderPaths[i]);
		else if (shaderPaths[i].ends_with("GS.glsl"))
			prepareShader(&geometryShader, GL_GEOMETRY_SHADER, shaderPaths[i]);
	}

	program = glCreateProgram();

	for (size_t i = 0; i < shaders.size(); i++)
	{
		if (*shaders[i] != -1)
			glAttachShader(program, *shaders[i]);
	}

	glLinkProgram(program);

	// get linking log
	GLint bufferLen;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufferLen);
	if (bufferLen > 0)
	{
		char *buffer = new char[bufferLen];
		glGetProgramInfoLog(program, bufferLen, nullptr, buffer);
		std::cerr << buffer << std::endl;
		delete[]buffer;
		glDeleteProgram(program);
		program = 0;
	}

	for (size_t i = 0; i < shaders.size(); i++)
	{
		if (*shaders[i] != -1)
			glDeleteShader(*shaders[i]);
	}
}

GLProgram::~GLProgram()
{
}

GLuint GLProgram::id()
{
	return program;
}

void GLProgram::use()
{
	glUseProgram(program);
}

void GLProgram::prepareShader(GLuint *shader, GLuint shaderType, std::string path)
{
	GLuint createdShader = glCreateShader(shaderType);
	const GLchar *vertexString = (GLchar *) SDL_LoadFile(path.c_str(), nullptr);
	glShaderSource(createdShader, 1, &vertexString, nullptr);
	glCompileShader(createdShader);

	*shader = createdShader;
}