#include "Model.hpp"

Model::Model(std::string path)
{
	bool ret = false;

	if (path.ends_with(".gltf"))
		ret = loader.LoadASCIIFromFile(&loadedModel, &err, &warn, path);
	else if (path.ends_with(".glb"))
		ret = loader.LoadBinaryFromFile(&loadedModel, &err, &warn, path);
	else
		printf("Wrong file format\n");

	if (!warn.empty()) {
		printf("Warn: %s\n", warn.c_str());
	}

	if (!err.empty()) {
		printf("Err: %s\n", err.c_str());
	}

	if (!ret) {
		printf("Failed to parse glTF\n");
	}
	loadTextures();

	int startingNodeId = loadedModel.scenes[loadedModel.defaultScene].nodes[0];
	processNode(&loadedModel.nodes[startingNodeId]);
}

Model::~Model()
{
}

void Model::loadTextures()
{
	Renderer* renderer = Renderer::getInstance();
	for (size_t i = 0; i < loadedModel.images.size(); i++)
	{
		renderer->loadTexture(&loadedModel.images[i]);
	}
}

void Model::processNode(tinygltf::Node *node)
{
	if (node->mesh > -1)
	{
		Mesh *mesh = new Mesh(&loadedModel.meshes[node->mesh], &loadedModel);
		meshes.push_back(mesh);
	}

	for (size_t i = 0; i < node->children.size(); i++)
	{
		processNode(&loadedModel.nodes[node->children[i]]);
	}
}

void Model::render()
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i]->render();
	}
}