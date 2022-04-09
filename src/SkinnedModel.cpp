#include "SkinnedModel.hpp"

glm::mat4 getMat4FromFloatPtr(float* ptr);

SkinnedModel::SkinnedModel(std::string path)
{
	loadFile(path);
	loadTextures();

	startingNodeId = currentNodeId = loadedModel.scenes[loadedModel.defaultScene].nodes[0];
	assert(!loadedModel.skins.empty());
	inverseBMAccessorId = loadedModel.skins[0].inverseBindMatrices;
	inverseBMPtr = (float*) Mesh::getDataPtr(nullptr, inverseBMAccessorId, &loadedModel);
	processNode(&loadedModel.nodes[startingNodeId], glm::mat4{ 1.0f });
}

void SkinnedModel::render()
{
	std::ranges::for_each(meshes, [](SkinnedMesh* m) { m->render(); });
}

void SkinnedModel::processNode(tinygltf::Node* node, glm::mat4 parentTransform)
{
	glm::mat4 nodeGlobalTransform = parentTransform * getTRSMatrix(&node->translation, &node->rotation, &node->scale);

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
		bone.inverseBindMatrix = getMat4FromFloatPtr(inverseBMPtr);
		bone.globalTransform = nodeGlobalTransform;

		bones.push_back(bone);
	}

	std::ranges::for_each(node->children, [&](int childId)
	{
		currentNodeId = childId;
		processNode(&loadedModel.nodes[childId], nodeGlobalTransform);
	});
}

glm::mat4 getMat4FromFloatPtr(float* ptr)
{
	std::vector<float> m;
	for (size_t i = 0; i < 16; i++)
		m.push_back(*ptr++);

	return glm::mat4{ m.at(0) , m.at(1) , m.at(2) , m.at(3) ,
					  m.at(4) , m.at(5) , m.at(6) , m.at(7) ,
					  m.at(8) , m.at(9) , m.at(10), m.at(11),
					  m.at(12), m.at(13), m.at(14), m.at(15) };
}