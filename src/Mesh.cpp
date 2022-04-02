#include "Mesh.hpp"


Mesh::Mesh(tinygltf::Mesh *mesh, tinygltf::Model *loadedModel)
{
	// Accessors
	int const positionsAccessorId	= mesh->primitives[0].attributes["POSITION"];
	int const normalsAccessorId		= mesh->primitives[0].attributes["NORMAL"];
	int const texCoords0AccessorId	= mesh->primitives[0].attributes["TEXCOORD_0"];
	int const indicesAccessorId		= mesh->primitives[0].indices;

	int numVertices = loadedModel->accessors[positionsAccessorId].count;
	int numIndices  = loadedModel->accessors[indicesAccessorId].count;
	int numPositionBytes;
	int numNormalBytes;
	int numTexCoords0Bytes;
	int numIndicesBytes;

	// WARNING: Assuming data is stored in buffers sequentally!!! 
	float *positionsPtr	 = (float *) getDataPtr(&numPositionBytes	, positionsAccessorId	, loadedModel);
	float *normalsPtr	 = (float *) getDataPtr(&numNormalBytes		, normalsAccessorId		, loadedModel);
	float *texCoords0Ptr = (float *) getDataPtr(&numTexCoords0Bytes	, texCoords0AccessorId	, loadedModel);
	unsigned short *indicesPtr = (unsigned short *)	getDataPtr(&numIndicesBytes, indicesAccessorId, loadedModel);

	for (size_t i = 0; i < numVertices; i++)
	{
		Vertex vertex = {};
		vertex.position.x	= *positionsPtr++;
		vertex.position.y	= *positionsPtr++;
		vertex.position.z	= *positionsPtr++;
		vertex.normal.x		= *normalsPtr++;
		vertex.normal.y		= *normalsPtr++;
		vertex.normal.z		= *normalsPtr++;
		vertex.texCoords.x  = *texCoords0Ptr++;
		vertex.texCoords.y  = *texCoords0Ptr++;
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

Mesh::~Mesh()
{
}

void * Mesh::getDataPtr(int *bytes, int accessorId, tinygltf::Model *loadedModel)
{
	// BufferView
	const int bufferViewId = loadedModel->accessors[accessorId].bufferView;
	const tinygltf::BufferView *bufferView = &loadedModel->bufferViews[bufferViewId];
	const int bufferId   = bufferView->buffer;
	const int byteLength = bufferView->byteLength;
	const int byteOffset = bufferView->byteOffset;

	// Buffer
	const tinygltf::Buffer *buffer = &loadedModel->buffers[bufferId];

	*bytes = byteLength;

	return (void *) &buffer->data[byteOffset];
}

/*
  Copies bytes given by accessor and returns pointer to that new memory
*/
void * Mesh::copyBufferData(int accessorId, tinygltf::Model *loadedModel)
{
	// BufferView
	const int bufferViewId = loadedModel->accessors[accessorId].bufferView;
	const tinygltf::BufferView *bufferView = &loadedModel->bufferViews[bufferViewId];
	const int bufferId   = bufferView->buffer;
	const int byteLength = bufferView->byteLength;
	const int byteOffset = bufferView->byteOffset;

	// Buffer
	const tinygltf::Buffer *buffer = &loadedModel->buffers[bufferId];

	// Allocate memory for the new buffer
	void *dataPtr = (void *) std::malloc(byteLength);
	assert(dataPtr);

	// Copy data
	std::memcpy(dataPtr, (const void *) &buffer->data[byteOffset], byteLength);

	return dataPtr;
}

void Mesh::loadMaterial(tinygltf::Mesh* mesh, tinygltf::Model* loadedModel)
{
	tinygltf::Material* m = &loadedModel->materials[mesh->primitives[0].material];

	int colorTextureId = m->pbrMetallicRoughness.baseColorTexture.index;
	int colorTextureImageId = loadedModel->textures[colorTextureId].source;
	material.colorTexture = loadedModel->images[colorTextureImageId].name;

	int normalTextureId = m->normalTexture.index;
	int normalTextureImageId = loadedModel->textures[normalTextureId].source;
	material.normalTexture = loadedModel->images[normalTextureImageId].name;
}

void Mesh::initOpenGLBuffers()
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

	// indices
	glVertexArrayElementBuffer(vao, ebo);
}

void Mesh::render()
{
	Renderer* renderer = Renderer::getInstance();

	GLProgram* program = renderer->useProgram(Renderer::Program::MESH);
	program->setUniform("uModelMatrix", glm::mat4(1.0f));
	program->setUniform("uViewMatrix", renderer->getCamera()->getViewMatrix());
	program->setUniform("uProjectionMatrix", renderer->getCamera()->getProjectionMatrix());
	program->setUniform("uCameraPos", renderer->getCamera()->getPosition());
	program->setUniform("uLightPos", glm::vec3{ 0.0f, 2.0f, 1.0f });
	program->setUniform("uLightColor", glm::vec3{ 1.0f, 1.0f, 1.0f });

	renderer->bindTexture(material.colorTexture, 0);
	renderer->bindTexture(material.normalTexture, 1);
	program->setUniform("colorTexture", 0);
	program->setUniform("normalTexture", 1);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}
