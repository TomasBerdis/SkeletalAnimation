#pragma once

#include <string>
#include <algorithm>

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
	glm::mat4 getInverseBindMatrix(unsigned int boneId);


private:
	std::vector<SkinnedMesh*> meshes;
	std::vector<Bone> armature;
	int startingNodeId = -1;
	int currentNodeId = -1;
	std::vector<glm::mat4> inverseBindMatrices;

	void loadInverseBindMatrices();
	void processNode(tinygltf::Node* node, glm::mat4 parentTransform);
};