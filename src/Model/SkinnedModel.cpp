#include "SkinnedModel.hpp"

SkinnedModel::SkinnedModel(std::string path)
{
	gltfUtil::loadFile(path, &loadedModel);
	loadTextures();

	startingNodeId = currentNodeId = loadedModel.scenes[loadedModel.defaultScene].nodes[0];
	assert(!loadedModel.skins.empty());
	inverseBMAccessorId = loadedModel.skins[0].inverseBindMatrices;
	inverseBMPtr = (float*) gltfUtil::getDataPtr(nullptr, inverseBMAccessorId, &loadedModel);
	processNode(&loadedModel.nodes[startingNodeId], glm::mat4{ 1.0f });
}

void SkinnedModel::render()
{
	std::ranges::for_each(meshes, [](SkinnedMesh* m) { m->render(); });
}

void SkinnedModel::processNode(tinygltf::Node* node, glm::mat4 parentTransform)
{
	glm::mat4 nodeGlobalTransform = parentTransform * gltfUtil::getTRSMatrix(&node->translation, &node->rotation, &node->scale);

	if (node->mesh > -1)
	{
		std::ranges::for_each(loadedModel.meshes[node->mesh].primitives, [&](tinygltf::Primitive primitive)
		{
			SkinnedMesh* mesh = new SkinnedMesh(&primitive, &loadedModel, nodeGlobalTransform);
			meshes.push_back(mesh);
		});
	}

	if (std::ranges::any_of(loadedModel.skins[0].joints, [&](int i) { return i == currentNodeId; }))
	{
		Bone bone;
		bone.id = currentNodeId;
		bone.name = node->name;
		bone.inverseBindMatrix = gltfUtil::getMat4FromFloatPtr(inverseBMPtr);
		bone.globalTransform = nodeGlobalTransform;

		armature.push_back(bone);
	}

	std::ranges::for_each(node->children, [&](int childId)
	{
		currentNodeId = childId;
		processNode(&loadedModel.nodes[childId], nodeGlobalTransform);
	});
}