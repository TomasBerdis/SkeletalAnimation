#pragma once

#include <string>

#include "Model.hpp"
#include "SkinnedMesh.hpp"

struct Bone
{
	unsigned int id;
	std::string name;
	glm::mat4 inverseBindMatrix;
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

	void processNode(tinygltf::Node* node);
	void processBones();
};