#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "tinyGLTF/tiny_gltf.h"
#include "Mesh.hpp"

class Model
{
public:
	Model(std::string path);
	Model() {};
	~Model();

	void virtual render();


protected:
	void loadFile(std::string path);
	void loadTextures();

	tinygltf::Model loadedModel;
	tinygltf::TinyGLTF loader;

private:
	std::vector<Mesh*> meshes;

	void processNode(tinygltf::Node* node, glm::mat4 parentTransform);
	glm::mat4 getTRSMatrix(std::vector<double>* translation, std::vector<double>* rotation, std::vector<double>* scale);
};