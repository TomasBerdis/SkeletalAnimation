#include "SkinnedMesh.hpp"

SkinnedMesh::SkinnedMesh(tinygltf::Primitive* primitive, tinygltf::Model* loadedModel, glm::mat4 globalTransform)
{
	// Accessors
	int const positionsAccessorId	= primitive->attributes["POSITION"];
	int const normalsAccessorId		= primitive->attributes["NORMAL"];
	int const texCoords0AccessorId	= primitive->attributes["TEXCOORD_0"];
	int const tangentAccessorId		= primitive->attributes["TANGENT"];
	int const jointsAccessorId		= primitive->attributes["JOINTS_0"];
	int const weightsAccessorId		= primitive->attributes["WEIGHTS_0"];
	int const indicesAccessorId		= primitive->indices;

	int numVertices = loadedModel->accessors[positionsAccessorId].count;
	int numIndices = loadedModel->accessors[indicesAccessorId].count;

	// WARNING: Assuming data is stored in buffers sequentally!!! 
	float* positionsPtr			= (float*)			getDataPtr(nullptr, positionsAccessorId		, loadedModel);
	float* normalsPtr			= (float*)			getDataPtr(nullptr, normalsAccessorId		, loadedModel);
	float* texCoords0Ptr		= (float*)			getDataPtr(nullptr, texCoords0AccessorId	, loadedModel);
	float* tangentPtr			= (float*)			getDataPtr(nullptr, tangentAccessorId		, loadedModel);
	unsigned char* jointsPtr	= (unsigned char*)	getDataPtr(nullptr, jointsAccessorId		, loadedModel);
	float* weightsPtr			= (float*)			getDataPtr(nullptr, weightsAccessorId		, loadedModel);
	unsigned short* indicesPtr	= (unsigned short*)	getDataPtr(nullptr, indicesAccessorId		, loadedModel);

	for (size_t i = 0; i < numVertices; i++)
	{
		Vertex vertex = {};
		vertex.position.x = *positionsPtr++;
		vertex.position.y = *positionsPtr++;
		vertex.position.z = *positionsPtr++;
		vertex.normal.x = *normalsPtr++;
		vertex.normal.y = *normalsPtr++;
		vertex.normal.z = *normalsPtr++;
		vertex.texCoords.x = *texCoords0Ptr++;
		vertex.texCoords.y = *texCoords0Ptr++;
		vertex.tangent.x = *tangentPtr++;
		vertex.tangent.y = *tangentPtr++;
		vertex.tangent.z = *tangentPtr++;
		vertex.tangent.w = *tangentPtr++;
		vertex.boneIds.x = *jointsPtr++;
		vertex.boneIds.y = *jointsPtr++;
		vertex.boneIds.z = *jointsPtr++;
		vertex.boneIds.w = *jointsPtr++;
		vertex.boneWeights.x = *weightsPtr++;
		vertex.boneWeights.y = *weightsPtr++;
		vertex.boneWeights.z = *weightsPtr++;
		vertex.boneWeights.w = *weightsPtr++;
		vertices.push_back(vertex);
	}

	for (size_t i = 0; i < numIndices; i++)
	{
		unsigned short index = *indicesPtr++;
		indices.push_back(index);
	}

	loadMaterial(primitive, loadedModel);
	initOpenGLBuffers();
}

SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::initOpenGLBuffers()
{
	// VBO
	glCreateBuffers(1, &vbo);
	glNamedBufferData(vbo, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glCreateBuffers(1, &ebo);
	glNamedBufferData(ebo, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glCreateVertexArrays(1, &vao);

	// position
	glVertexArrayAttribBinding(vao, 0, 0);
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, 0, vbo, (GLintptr) 0, sizeof(Vertex));

	// normal
	glVertexArrayAttribBinding(vao, 1, 1);
	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, 1, vbo, (GLintptr) offsetof(Vertex, normal), sizeof(Vertex));

	// texCoords
	glVertexArrayAttribBinding(vao, 2, 2);
	glEnableVertexArrayAttrib(vao, 2);
	glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, 2, vbo, (GLintptr) offsetof(Vertex, texCoords), sizeof(Vertex));

	// tangent
	glVertexArrayAttribBinding(vao, 3, 3);
	glEnableVertexArrayAttrib(vao, 3);
	glVertexArrayAttribFormat(vao, 3, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, 3, vbo, (GLintptr)offsetof(Vertex, tangent), sizeof(Vertex));

	// bone ids
	glVertexArrayAttribBinding(vao, 4, 4);
	glEnableVertexArrayAttrib(vao, 4);
	glVertexArrayAttribIFormat(vao, 4, 4, GL_INT, 0);
	glVertexArrayVertexBuffer(vao, 4, vbo, (GLintptr) offsetof(Vertex, boneIds), sizeof(Vertex));

	// bone weights
	glVertexArrayAttribBinding(vao, 5, 5);
	glEnableVertexArrayAttrib(vao, 5);
	glVertexArrayAttribFormat(vao, 5, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, 5, vbo, (GLintptr) offsetof(Vertex, boneWeights), sizeof(Vertex));

	// indices
	glVertexArrayElementBuffer(vao, ebo);
}

void SkinnedMesh::render()
{
	Renderer* renderer = Renderer::getInstance();

	GLProgram* program = renderer->useProgram(Renderer::Program::MESH);
	program->setUniform("uModelMatrix", globalTransform);
	program->setUniform("uViewMatrix", renderer->getCamera()->getViewMatrix());
	program->setUniform("uProjectionMatrix", renderer->getCamera()->getProjectionMatrix());
	program->setUniform("uNormalMatrix", glm::transpose(glm::inverse(glm::mat3(1.0f))));

	program->setUniform("uCameraPos", renderer->getCamera()->getPosition());
	program->setUniform("uLightPos", glm::vec3{ 0.0f, 2.0f, 1.0f });
	program->setUniform("uLightColor", glm::vec3{ 1.0f, 1.0f, 1.0f });

	renderer->bindTexture(material.colorTexture, 0);
	if (material.normalTexture != "")
	{
		renderer->bindTexture(material.normalTexture, 1);
		program->setUniform("uUseNormalMap", true);
	}
	else
		program->setUniform("uUseNormalMap", false);

	program->setUniform("diffuseMap", 0);
	program->setUniform("normalMap", 1);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}