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

struct Vertex
{
	glm::vec3	position;
	glm::vec3	normal;
	glm::vec2	texCoords;
};

class Mesh
{
public:
	Mesh(tinygltf::Mesh *mesh, tinygltf::Model *loadedModel);
	~Mesh();

	void render();

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;

	GLuint vao, vbo, ebo;

	void *getDataPtr(int *bytes, int accessorId, tinygltf::Model *loadedModel);
	void *copyBufferData(int accessorId, tinygltf::Model *loadedModel);
	void initOpenGLBuffers();
};