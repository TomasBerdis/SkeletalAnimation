#pragma once

#include "../GltfUtil.hpp"
#include "GLEW/glew.h"
#include "SDL2/SDL_opengl.h"
#include "glm/glm.hpp"
#include "../Renderer.hpp"
#include "../GL/GLProgram.hpp"

#include <stdlib.h>
#include <string.h>
#include <vector>


struct Material
{
	std::string colorTexture = "";
	std::string normalTexture = "";
};

class Mesh
{
public:
	Mesh(tinygltf::Primitive* const primitive, tinygltf::Model* const loadedModel, const glm::mat4 globalTransform);
	Mesh() {};
	~Mesh();

	void virtual render();
	void updateGlobalTransform(const glm::mat4 matrix);
	void updateProgramType(Renderer::Program programType);

protected:
	std::vector<unsigned short> indices;
	glm::mat4 globalTransform = glm::mat4(1.0f);
	Renderer::Program programType = Renderer::Program::MESH;
	Material material;
	GLuint vao = 0, vbo = 0, ebo = 0;
	struct Vertex
	{
		glm::vec3	position;
		glm::vec3	normal;
		glm::vec2	texCoords;
		glm::vec4	tangent;	// tangent.w is the bitangent sign
	};

	void loadMaterial(tinygltf::Primitive* const primitive, tinygltf::Model* const loadedModel);
	void initOpenGLBuffers();

private:
	std::vector<Vertex> vertices;
};