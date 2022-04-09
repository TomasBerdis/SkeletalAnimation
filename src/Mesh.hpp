#pragma once

#include "tinyGLTF/tiny_gltf.h"
#include "GLEW/glew.h"
#include "SDL2/SDL_opengl.h"
#include "glm/glm.hpp"
#include "Renderer.hpp"
#include "GL/GLProgram.hpp"

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
	Mesh(tinygltf::Primitive *primitive, tinygltf::Model *loadedModel, glm::mat4 globalTransform);
	Mesh() {};
	~Mesh();

	void virtual render();
	static void* getDataPtr(int* bytes, int accessorId, tinygltf::Model* loadedModel);
	static void* copyBufferData(int accessorId, tinygltf::Model* loadedModel);

protected:
	std::vector<unsigned short> indices;
	glm::mat4 globalTransform;
	Material material;
	GLuint vao, vbo, ebo;
	struct Vertex
	{
		glm::vec3	position;
		glm::vec3	normal;
		glm::vec2	texCoords;
		glm::vec4	tangent;	// tangent.w is the bitangent sign
	};

	void loadMaterial(tinygltf::Primitive* primitive, tinygltf::Model* loadedModel);
	void initOpenGLBuffers();

private:
	std::vector<Vertex> vertices;
};