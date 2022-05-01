#include "GLProgram.hpp"


GLProgram::GLProgram(const std::vector<std::string> shaderPaths)
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
		std::cerr << buffer << '\n';
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

void GLProgram::prepareShader(GLuint* shader, GLuint shaderType, const std::string path)
{
	GLuint createdShader = glCreateShader(shaderType);
	const GLchar *vertexString = (GLchar *) SDL_LoadFile(path.c_str(), nullptr);
	glShaderSource(createdShader, 1, &vertexString, nullptr);
	glCompileShader(createdShader);

	*shader = createdShader;
}

template <typename T>
void GLProgram::setUniformv(const GLchar* uniformName, T uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	std::string typeName = typeid(T).name();
	glProgramUniform1uiv(program, uniformLocation, (GLsizei)sizeof(uniformValue) / sizeof(uniformValue[0]), uniformValue);

	if (typeName == "float * __ptr64")
		glProgramUniform1fv(program, uniformLocation, (GLsizei)sizeof(uniformValue) / sizeof(uniformValue[0]), reinterpret_cast <GLfloat*> (uniformValue));
	else if (typeName == "double * __ptr64")
		glProgramUniform1dv(program, uniformLocation, (GLsizei)sizeof(uniformValue) / sizeof(uniformValue[0]), reinterpret_cast <GLdouble*> (uniformValue));
	else if (typeName == "unsigned int * __ptr64")
		glProgramUniform1uiv(program, uniformLocation, (GLsizei)sizeof(uniformValue) / sizeof(uniformValue[0]), reinterpret_cast <GLuint*> (uniformValue));
	else if (typeName == "int * __ptr64")
		glProgramUniform1iv(program, uniformLocation, (GLsizei)sizeof(uniformValue) / sizeof(uniformValue[0]), reinterpret_cast <GLint*> (uniformValue));
}

void GLProgram::setUniform(const GLchar* uniformName, bool uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	glProgramUniform1i(program, uniformLocation, uniformValue);
}

void GLProgram::setUniform(const GLchar* uniformName, glm::mat3 uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	glProgramUniformMatrix3fv(program, uniformLocation, 1, GL_FALSE, glm::value_ptr(uniformValue));
}

void GLProgram::setUniform(const GLchar* uniformName, glm::mat4 uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	glProgramUniformMatrix4fv(program, uniformLocation, 1, GL_FALSE, glm::value_ptr(uniformValue));
}

void GLProgram::setUniform(const GLchar* uniformName, glm::vec2 uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	glProgramUniform2f(program, uniformLocation, uniformValue.x, uniformValue.y);
}

void GLProgram::setUniform(const GLchar* uniformName, glm::vec3 uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	glProgramUniform3f(program, uniformLocation, uniformValue.x, uniformValue.y, uniformValue.z);
}

void GLProgram::setUniform(const GLchar* uniformName, glm::vec4 uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	glProgramUniform4f(program, uniformLocation, uniformValue.x, uniformValue.y, uniformValue.z, uniformValue.w);
}

void GLProgram::setUniform(const GLchar* uniformName, glm::ivec2 uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	glProgramUniform2i(program, uniformLocation, uniformValue.x, uniformValue.y);
}

void GLProgram::setUniform(const GLchar* uniformName, glm::ivec3 uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	glProgramUniform3i(program, uniformLocation, uniformValue.x, uniformValue.y, uniformValue.z);
}

void GLProgram::setUniform(const GLchar* uniformName, glm::ivec4 uniformValue)
{
	use();
	GLint uniformLocation = glGetUniformLocation(program, uniformName);
	glProgramUniform4i(program, uniformLocation, uniformValue.x, uniformValue.y, uniformValue.z, uniformValue.w);
}

void GLProgram::setMatrixArrayUniform(const GLchar* uniformName, std::vector<glm::mat4>* vec)
{
	for (size_t i = 0; i < vec->size(); i++)
	{
		std::string s = std::string(uniformName) + "[" + std::to_string(i) + "]";
		GLint uniformLocation = glGetUniformLocation(program, s.c_str());
		glProgramUniformMatrix4fv(program, uniformLocation, 1, GL_FALSE, glm::value_ptr(vec->at(i)));
	}
}