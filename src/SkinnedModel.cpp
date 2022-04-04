#include "SkinnedModel.hpp"

SkinnedModel::SkinnedModel(std::string path)
{
	loadFile(path);
	loadTextures();

	startingNodeId = loadedModel.scenes[loadedModel.defaultScene].nodes[0];
	processNode(&loadedModel.nodes[startingNodeId], glm::mat4{ 1.0f });
}

void SkinnedModel::render()
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i]->render();
	}
}


void SkinnedModel::processNode(tinygltf::Node* node, glm::mat4 parentTransform)
{
	glm::mat4 nodeGlobalTransform = parentTransform * getTRSMatrix(&node->translation, &node->rotation, &node->scale);

	if (node->mesh > -1)
	{
		SkinnedMesh* mesh = new SkinnedMesh(&loadedModel.meshes[node->mesh], &loadedModel, nodeGlobalTransform);
		meshes.push_back(mesh);
	}

	for (size_t i = 0; i < node->children.size(); i++)
	{
		processNode(&loadedModel.nodes[node->children[i]], nodeGlobalTransform);
	}
}

void SkinnedModel::processBones()
{
	for (size_t i = 0; i < loadedModel.skins[0].joints.size(); i++)
	{
		int nodeId = loadedModel.skins[0].joints[i];

		Bone bone;
		bone.id = nodeId;
		bone.name = loadedModel.nodes[nodeId].name;
	}
}