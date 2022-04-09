#pragma once

#include <string>

#include "Model.hpp"
#include "SkinnedMesh.hpp"

struct Bone
{
	unsigned int id;
	std::string name;
	glm::mat4 inverseBindMatrix;
	glm::mat4 globalTransform;
};

class SkinnedModel : public Model
{
public:
	SkinnedModel(std::string path);

	void render();

private:
	std::vector<SkinnedMesh*> meshes;
	std::vector<Bone> bones;
	int startingNodeId = -1;
	int currentNodeId = -1;
	int inverseBMAccessorId = -1;
	float* inverseBMPtr;

	void processNode(tinygltf::Node* node, glm::mat4 parentTransform);
};