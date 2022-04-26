#pragma once

#include <string>
#include <algorithm>

#include "Model.hpp"
#include "SkinnedMesh.hpp"

struct Bone
{
	std::string name;
	glm::mat4 inverseBindMatrix;
};

class SkinnedModel : public Model
{
public:
	SkinnedModel(std::string path);

	void render();
	std::vector<Bone>* getArmature();


private:
	std::vector<SkinnedMesh*> meshes;
	std::vector<Bone> armature;
	int startingNodeId = -1;
	int currentNodeId = -1;

	void loadArmature();
	void processNode(tinygltf::Node* node, glm::mat4 parentTransform);
};