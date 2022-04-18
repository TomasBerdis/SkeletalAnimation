#include "Model.hpp"

Model::Model(std::string path)
{
	gltfUtil::loadFile(path, &loadedModel);
	loadTextures();

	int startingNodeId = loadedModel.scenes[loadedModel.defaultScene].nodes[0];
	processNode(&loadedModel.nodes[startingNodeId], glm::mat4 {1.0f});
}

Model::~Model()
{
}

void Model::loadTextures()
{
	Renderer* renderer = Renderer::getInstance();
	std::ranges::for_each(loadedModel.images, [&](tinygltf::Image i) { renderer->loadTexture(&i); });
}

void Model::processNode(tinygltf::Node *node, glm::mat4 parentTransform)
{
	glm::mat4 nodeGlobalTransform = parentTransform * gltfUtil::getTRSMatrix(&node->translation, &node->rotation, &node->scale);

	if (node->mesh > -1)
	{
		tinygltf::Mesh* meshPtr = &loadedModel.meshes[node->mesh];
		for (size_t i = 0; i < meshPtr->primitives.size(); i++)
		{
			Mesh* mesh = new Mesh(&meshPtr->primitives[i], &loadedModel, nodeGlobalTransform);
			meshes.push_back(mesh);
		}
	}

	std::ranges::for_each(node->children, [&](int i) { processNode(&loadedModel.nodes[i], nodeGlobalTransform); });
}

void Model::render()
{
	std::ranges::for_each(meshes, [](Mesh* m) { m->render(); });
}

void Model::translate(glm::vec3 translation)
{
	modelMatrix = glm::translate(modelMatrix, translation);
	updateMeshTransforms();
}

void Model::rotate(glm::quat rotation)
{
	modelMatrix *= glm::mat4_cast(rotation);
	updateMeshTransforms();
}

void Model::scale(glm::vec3 scale)
{
	modelMatrix = glm::scale(modelMatrix, scale);
	updateMeshTransforms();
}

void Model::resetModelMatrix()
{
	modelMatrix = glm::mat4{ 1.0f };
	updateMeshTransforms();
}

void Model::setModelMatrix(glm::mat4 matrix)
{
	modelMatrix = matrix;
	updateMeshTransforms();
}

void Model::updateMeshTransforms()
{
	std::ranges::for_each(meshes, [&](Mesh* m) { m->updateGlobalTransform(modelMatrix); });
}

void Model::updateProgramType(Renderer::Program programType)
{
	std::ranges::for_each(meshes, [&](Mesh* m) { m->updateProgramType(programType); });
}
