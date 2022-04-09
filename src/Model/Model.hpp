#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "Mesh.hpp"

class Model
{
public:
	Model(std::string path);
	Model() {};
	~Model();

	void virtual render();


protected:
	tinygltf::Model loadedModel;

	void loadTextures();


private:
	std::vector<Mesh*> meshes;

	void processNode(tinygltf::Node* node, glm::mat4 parentTransform);
};