#pragma once

#include <string>
#include <algorithm>

#include "Model.hpp"
#include "SkinnedMesh.hpp"

struct Bone
{
	std::string name;
	glm::mat4 inverseBindMatrix = glm::mat4(1.0f);
};

class SkinnedModel : public Model
{
public:
	SkinnedModel(const std::string path);
	SkinnedModel(tinygltf::Model* const file);
	virtual ~SkinnedModel();

	void render();
	std::vector<Bone>* getArmature();


private:
	std::vector<SkinnedMesh*> meshes;
	std::vector<Bone> armature;
	int32_t currentNodeId = -1;

	void loadArmature();
	void processNode(const tinygltf::Node* const node, const glm::mat4 parentTransform);
};