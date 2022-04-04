#pragma once

#include "Mesh.hpp"

class SkinnedMesh : Mesh
{
public:
	SkinnedMesh(tinygltf::Mesh* mesh, tinygltf::Model* loadedModel, glm::mat4 globalTransform);
	~SkinnedMesh();

	void render();

private:
	struct Vertex
	{
		glm::vec3	position;
		glm::vec3	normal;
		glm::vec2	texCoords;
		glm::vec4	tangent;
		glm::ivec4	boneIds;
		glm::vec4	boneWeights;
	};
	std::vector<Vertex> vertices;

	void initOpenGLBuffers();
};