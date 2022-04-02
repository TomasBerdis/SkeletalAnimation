#pragma once

#include <string>

#include "Model.hpp"
#include "SkinnedMesh.hpp"

class SkinnedModel : public Model
{
public:
	SkinnedModel(std::string path);

	void render();

private:
	std::vector<SkinnedMesh*> meshes;

	void processNode(tinygltf::Node* node);
};