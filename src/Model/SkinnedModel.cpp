#include "SkinnedModel.hpp"

SkinnedModel::SkinnedModel(std::string path)
{
	gltfUtil::loadFile(path, &loadedModel);
	loadTextures();

	startingNodeId = currentNodeId = loadedModel.scenes[loadedModel.defaultScene].nodes[0];
	assert(!loadedModel.skins.empty());

	loadArmature();
	processNode(&loadedModel.nodes[startingNodeId], glm::mat4{ 1.0f });

	loadedModel.~Model();
}

void SkinnedModel::render()
{
	std::ranges::for_each(meshes, [](SkinnedMesh* m) { m->render(); });
}

std::vector<Bone>* SkinnedModel::getArmature()
{
	return &armature;
}

void SkinnedModel::loadArmature()
{
	int inverseBMAccessorId = loadedModel.skins[0].inverseBindMatrices;
	float* inverseBMPtr = (float*) gltfUtil::getDataPtr(nullptr, inverseBMAccessorId, &loadedModel);

	for (size_t i = 0; i < loadedModel.skins[0].joints.size(); i++)
	{
		int nodeId = loadedModel.skins[0].joints[i];
		Bone b;
		b.name = loadedModel.nodes[nodeId].name;
		b.inverseBindMatrix = gltfUtil::getMat4FromFloatPtr(inverseBMPtr);
		inverseBMPtr += 16;
		armature.push_back(b);
	}
}

void SkinnedModel::processNode(tinygltf::Node* node, glm::mat4 parentTransform)
{
	glm::mat4 nodeGlobalTransform = parentTransform * gltfUtil::getTRSMatrix(&node->translation, &node->rotation, &node->scale);

	// if this node is a mesh
	if (node->mesh > -1)
	{
		std::ranges::for_each(loadedModel.meshes[node->mesh].primitives, [&](tinygltf::Primitive primitive)
		{
			SkinnedMesh* mesh = new SkinnedMesh(&primitive, &loadedModel, nodeGlobalTransform);
			meshes.push_back(mesh);
		});
	}

	std::ranges::for_each(node->children, [&](int childId)
	{
		currentNodeId = childId;
		processNode(&loadedModel.nodes[childId], nodeGlobalTransform);
	});
}