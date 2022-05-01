#include "Mesh.hpp"


Mesh::Mesh(tinygltf::Primitive* const primitive, tinygltf::Model* const loadedModel, const glm::mat4 globalTransform)
{
	this->programType = Renderer::Program::MESH;
	this->globalTransform = globalTransform;

	// Accessors
	const int32_t positionsAccessorId	= primitive->attributes["POSITION"];
	const int32_t normalsAccessorId		= primitive->attributes["NORMAL"];
	const int32_t texCoords0AccessorId	= primitive->attributes["TEXCOORD_0"];
	const int32_t tangentAccessorId		= primitive->attributes["TANGENT"];
	const int32_t indicesAccessorId		= primitive->indices;

	const int32_t numVertices = loadedModel->accessors[positionsAccessorId].count;
	const int32_t numIndices  = loadedModel->accessors[indicesAccessorId].count;

	// WARNING: Assuming data is stored in buffers sequentally!!! 
	float* positionsPtr	 = (float*) gltfUtil::getDataPtr(nullptr, positionsAccessorId	, loadedModel);
	float* normalsPtr	 = (float*) gltfUtil::getDataPtr(nullptr, normalsAccessorId		, loadedModel);
	float* texCoords0Ptr = (float*) gltfUtil::getDataPtr(nullptr, texCoords0AccessorId	, loadedModel);
	float* tangentPtr	 = (float*) gltfUtil::getDataPtr(nullptr, tangentAccessorId		, loadedModel);
	uint16_t *indicesPtr = (uint16_t*) gltfUtil::getDataPtr(nullptr, indicesAccessorId	, loadedModel);

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
		vertex.tangent.x	= *tangentPtr++;
		vertex.tangent.y	= *tangentPtr++;
		vertex.tangent.z	= *tangentPtr++;
		vertex.tangent.w	= *tangentPtr++;
		vertices.push_back(vertex);
	}

	for (size_t i = 0; i < numIndices; i++)
	{
		uint16_t index = *indicesPtr++;
		indices.push_back(index);
	}

	loadMaterial(primitive, loadedModel);
	initOpenGLBuffers();
}

Mesh::~Mesh()
{
}

void Mesh::loadMaterial(tinygltf::Primitive* const primitive, tinygltf::Model* const loadedModel)
{
	tinygltf::Material* m;
	if (!loadedModel->materials.empty())
		m = &loadedModel->materials[primitive->material];
	else
	{
		material.colorTexture = Renderer::debugTexture;
		return;
	}

	const int32_t colorTextureId = m->pbrMetallicRoughness.baseColorTexture.index;
	if (colorTextureId > -1)
	{
		const int32_t colorTextureImageId = loadedModel->textures[colorTextureId].source;
		material.colorTexture = loadedModel->images[colorTextureImageId].name;
	}
	else
		material.colorTexture = Renderer::debugTexture;

	const int32_t normalTextureId = m->normalTexture.index;
	if (normalTextureId > -1)
	{
		const int32_t normalTextureImageId = loadedModel->textures[normalTextureId].source;
		material.normalTexture = loadedModel->images[normalTextureImageId].name;
	}
}

void Mesh::initOpenGLBuffers()
{
	// VBO
	glCreateBuffers(1, &vbo);
	glNamedBufferData(vbo, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glCreateBuffers(1, &ebo);
	glNamedBufferData(ebo, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
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
	glVertexArrayVertexBuffer(vao, 3, vbo, (GLintptr) offsetof(Vertex, tangent), sizeof(Vertex));

	// indices
	glVertexArrayElementBuffer(vao, ebo);
}

void Mesh::render()
{
	Renderer* renderer = Renderer::getInstance();

	GLProgram* program = renderer->useProgram(programType);
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

void Mesh::updateGlobalTransform(const glm::mat4 matrix)
{
	globalTransform = matrix;
}

void Mesh::updateProgramType(Renderer::Program programType)
{
	this->programType = programType;
}
