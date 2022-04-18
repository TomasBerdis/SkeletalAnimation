#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "Mesh.hpp"

class Model
{
public:
	Model(std::string path);
	Model() {};
	~Model();

	void virtual render();
	void translate(glm::vec3 translation);
	void rotate(glm::quat rotation);
	void scale(glm::vec3 scale);
	void resetModelMatrix();
	void setModelMatrix(glm::mat4 matrix);
	void updateProgramType(Renderer::Program programType);


protected:
	tinygltf::Model loadedModel;

	void loadTextures();
	void updateMeshTransforms();


private:
	std::vector<Mesh*> meshes;
	glm::mat4 modelMatrix = glm::mat4 { 1.0f };

	void processNode(tinygltf::Node* node, glm::mat4 parentTransform);
};