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
	Mesh(tinygltf::Mesh *mesh, tinygltf::Model *loadedModel);
	Mesh() {};
	~Mesh();

	void virtual render();

protected:
	std::vector<unsigned short> indices;
	Material material;
	GLuint vao, vbo, ebo;
	struct Vertex
	{
		glm::vec3	position;
		glm::vec3	normal;
		glm::vec2	texCoords;
		glm::vec4	tangent;	// tangent.w is the bitangent sign
	};

	void* getDataPtr(int* bytes, int accessorId, tinygltf::Model* loadedModel);
	void* copyBufferData(int accessorId, tinygltf::Model* loadedModel);
	void loadMaterial(tinygltf::Mesh* mesh, tinygltf::Model* loadedModel);
	void initOpenGLBuffers();

private:
	std::vector<Vertex> vertices;
};