#include "SkinnedModel.hpp"

SkinnedModel::SkinnedModel(std::string path)
{
	gltfUtil::loadFile(path, &loadedModel);
	loadTextures();

	startingNodeId = currentNodeId = loadedModel.scenes[loadedModel.defaultScene].nodes[0];
	assert(!loadedModel.skins.empty());

	loadInverseBindMatrices();
	processNode(&loadedModel.nodes[startingNodeId], glm::mat4{ 1.0f });
}

void SkinnedModel::render()
{
	std::ranges::for_each(meshes, [](SkinnedMesh* m) { m->render(); });
}

glm::mat4 SkinnedModel::getInverseBindMatrix(unsigned int boneId)
{
	if (auto result = std::ranges::find_if(armature, [&](Bone b) { return b.id == boneId; }); result != armature.end())
		return result[0].inverseBindMatrix;
	else
		return glm::mat4 { 1.0f };
}

void SkinnedModel::loadInverseBindMatrices()
{
	int inverseBMAccessorId = loadedModel.skins[0].inverseBindMatrices;
	float* inverseBMPtr = (float*) gltfUtil::getDataPtr(nullptr, inverseBMAccessorId, &loadedModel);
	for (size_t i = 0; i < loadedModel.accessors[inverseBMAccessorId].count; i++)
	{
		inverseBindMatrices.push_back(gltfUtil::getMat4FromFloatPtr(inverseBMPtr));
		inverseBMPtr += 16;
	}
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

	if (auto result = std::ranges::find_if(loadedModel.skins[0].joints, [&](int i) { return i == currentNodeId; });
		result != loadedModel.skins[0].joints.end())
	{
		int indexInJoints = std::distance(std::begin(loadedModel.skins[0].joints), result);
		Bone bone;
		bone.id = currentNodeId;
		bone.name = node->name;
		bone.inverseBindMatrix = inverseBindMatrices[indexInJoints];
		bone.globalTransform = nodeGlobalTransform;

		armature.push_back(bone);
	}

	std::ranges::for_each(node->children, [&](int childId)
	{
		currentNodeId = childId;
		processNode(&loadedModel.nodes[childId], nodeGlobalTransform);
	});
}