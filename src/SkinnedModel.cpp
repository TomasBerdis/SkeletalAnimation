#include "SkinnedModel.hpp"

SkinnedModel::SkinnedModel(std::string path)
{
	loadFile(path);
	loadTextures();

	int startingNodeId = loadedModel.scenes[loadedModel.defaultScene].nodes[0];
	processNode(&loadedModel.nodes[startingNodeId]);
}

void SkinnedModel::render()
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i]->render();
	}
}


void SkinnedModel::processNode(tinygltf::Node* node)
{
	if (node->mesh > -1)
	{
		SkinnedMesh* mesh = new SkinnedMesh(&loadedModel.meshes[node->mesh], &loadedModel);
		meshes.push_back(mesh);
	}

	for (size_t i = 0; i < node->children.size(); i++)
	{
		processNode(&loadedModel.nodes[node->children[i]]);
	}
}