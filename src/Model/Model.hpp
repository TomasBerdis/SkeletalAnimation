#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "Mesh.hpp"

class Model
{
public:
	Model(const std::string path);
	Model(tinygltf::Model* const file);
	Model() {};
	virtual ~Model();

	void virtual render();
	void translate(const glm::vec3 translation);
	void rotate(const glm::quat rotation);
	void scale(const glm::vec3 scale);
	void resetModelMatrix();
	void setModelMatrix(glm::mat4 matrix);
	void updateProgramType(Renderer::Program programType);


protected:
	tinygltf::Model* loadedModel;

	void loadTextures();
	void updateMeshTransforms();


private:
	std::vector<Mesh*> meshes;
	glm::mat4 modelMatrix = glm::mat4 { 1.0f };

	void processNode(const tinygltf::Node* const node, const glm::mat4 parentTransform);
};