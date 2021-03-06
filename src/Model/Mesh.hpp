#pragma once

#include "../Renderer.hpp"
#include "../GL/GLProgram.hpp"
#include "../GltfUtil.hpp"
#include "SDL_opengl.h"
#include "glm.hpp"

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
	virtual ~Mesh();

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