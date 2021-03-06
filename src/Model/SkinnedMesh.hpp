#pragma once

#include "Mesh.hpp"

class SkinnedMesh : Mesh
{
public:
	SkinnedMesh(tinygltf::Primitive* const primitive, tinygltf::Model* const loadedModel, const glm::mat4 globalTransform);
	virtual ~SkinnedMesh();

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