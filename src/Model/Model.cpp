#include "Model.hpp"

Model::Model(std::string path)
{
	loadFile(path);
	loadTextures();

	int startingNodeId = loadedModel.scenes[loadedModel.defaultScene].nodes[0];
	processNode(&loadedModel.nodes[startingNodeId], glm::mat4 {1.0f});
}

Model::~Model()
{
}

void Model::loadFile(std::string path)
{
	bool ret = false;
	std::string err;
	std::string warn;

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
}

void Model::loadTextures()
{
	Renderer* renderer = Renderer::getInstance();
	std::ranges::for_each(loadedModel.images, [&](tinygltf::Image i) { renderer->loadTexture(&i); });
}

void Model::processNode(tinygltf::Node *node, glm::mat4 parentTransform)
{
	glm::mat4 nodeGlobalTransform = parentTransform * getTRSMatrix(&node->translation, &node->rotation, &node->scale);

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

glm::mat4 Model::getTRSMatrix(std::vector<double>* translation, std::vector<double>* rotation, std::vector<double>* scale)
{
	if (translation->empty() && rotation->empty() && scale->empty())
		return glm::mat4{ 1.0f };

	glm::mat4 T{ 1.0f };
	glm::mat4 R{ 1.0f };
	glm::mat4 S{ 1.0f };

	// WARNING: Potentially slow code
	if (!translation->empty())
		T = glm::translate(glm::mat4{ 1.0f }, { translation->at(0), translation->at(1), translation->at(2) });
	if (!rotation->empty())
		R = glm::mat4_cast(glm::quat((float)(rotation->at(3)), (float)(rotation->at(0)), (float)(rotation->at(1)), (float)(rotation->at(2))));
	if (!scale->empty())
		S = glm::scale(glm::mat4{ 1.0f }, { scale->at(0), scale->at(1), scale->at(2) });
	
	return T * R * S;
}