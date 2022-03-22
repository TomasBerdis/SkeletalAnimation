#pragma once

#include <string>
#include <vector>

#include "tinyGLTF/tiny_gltf.h"
#include "Mesh.hpp"

class Model
{
public:
	Model(std::string path);
	~Model();

	void render();

private:
	void processNode(tinygltf::Node *node);

	tinygltf::Model loadedModel;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;
	std::vector<Mesh *> meshes;
};