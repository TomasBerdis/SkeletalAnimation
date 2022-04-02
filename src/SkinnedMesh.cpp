#include "SkinnedMesh.hpp"

SkinnedMesh::SkinnedMesh(tinygltf::Mesh* mesh, tinygltf::Model* loadedModel)
{
	// Accessors
	int const positionsAccessorId	= mesh->primitives[0].attributes["POSITION"];
	int const normalsAccessorId		= mesh->primitives[0].attributes["NORMAL"];
	int const texCoords0AccessorId	= mesh->primitives[0].attributes["TEXCOORD_0"];
	int const jointsAccessorId		= mesh->primitives[0].attributes["JOINTS_0"];
	int const weightsAccessorId		= mesh->primitives[0].attributes["WEIGHTS_0"];
	int const indicesAccessorId		= mesh->primitives[0].indices;

	int numVertices = loadedModel->accessors[positionsAccessorId].count;
	int numIndices = loadedModel->accessors[indicesAccessorId].count;
	int numPositionBytes;
	int numNormalBytes;
	int numTexCoords0Bytes;
	int numJointsBytes;
	int numWeightsBytes;
	int numIndicesBytes;

	// WARNING: Assuming data is stored in buffers sequentally!!! 
	float* positionsPtr			= (float*)			getDataPtr(&numPositionBytes	, positionsAccessorId	, loadedModel);
	float* normalsPtr			= (float*)			getDataPtr(&numNormalBytes		, normalsAccessorId		, loadedModel);
	float* texCoords0Ptr		= (float*)			getDataPtr(&numTexCoords0Bytes	, texCoords0AccessorId	, loadedModel);
	unsigned char* jointsPtr	= (unsigned char*)	getDataPtr(&numJointsBytes		, jointsAccessorId		, loadedModel);
	float* weightsPtr			= (float*)			getDataPtr(&numWeightsBytes		, weightsAccessorId		, loadedModel);
	unsigned short* indicesPtr	= (unsigned short*)	getDataPtr(&numIndicesBytes		, indicesAccessorId		, loadedModel);

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

	loadMaterial(mesh, loadedModel);
	initOpenGLBuffers();
}

SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::render()
{
	Renderer* renderer = Renderer::getInstance();

	GLProgram* program = renderer->useProgram(Renderer::Program::MESH);
	program->setUniform("uModelMatrix"		, glm::mat4(1.0f));
	program->setUniform("uViewMatrix"		, renderer->getCamera()->getViewMatrix());
	program->setUniform("uProjectionMatrix"	, renderer->getCamera()->getProjectionMatrix());
	program->setUniform("uCameraPos", renderer->getCamera()->getPosition());
	program->setUniform("uLightPos", glm::vec3 { 0.0f, 2.0f, 1.0f });
	program->setUniform("uLightColor", glm::vec3 { 1.0f, 1.0f, 1.0f });

	renderer->bindTexture(material.colorTexture, 0);
	renderer->bindTexture(material.normalTexture, 1);
	program->setUniform("colorTexture", 0);
	program->setUniform("normalTexture", 1);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
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

	// bone ids
	glVertexArrayAttribBinding(vao, 3, 3);
	glEnableVertexArrayAttrib(vao, 3);
	glVertexArrayAttribIFormat(vao, 3, 4, GL_INT, 0);
	glVertexArrayVertexBuffer(vao, 3, vbo, (GLintptr) offsetof(Vertex, boneIds), sizeof(Vertex));

	// bone weights
	glVertexArrayAttribBinding(vao, 4, 4);
	glEnableVertexArrayAttrib(vao, 4);
	glVertexArrayAttribFormat(vao, 4, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, 4, vbo, (GLintptr) offsetof(Vertex, boneWeights), sizeof(Vertex));

	// indices
	glVertexArrayElementBuffer(vao, ebo);
}
