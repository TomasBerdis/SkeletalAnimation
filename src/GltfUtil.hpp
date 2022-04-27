#pragma once

#include "tinyGLTF/tiny_gltf.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>

namespace gltfUtil
{
	inline void loadFile(std::string path, tinygltf::Model* loadedModel)
	{
		bool ret = false;
		std::string err;
		std::string warn;
		tinygltf::TinyGLTF loader;

		if (path.ends_with(".gltf"))
			ret = loader.LoadASCIIFromFile(loadedModel, &err, &warn, path);
		else if (path.ends_with(".glb"))
			ret = loader.LoadBinaryFromFile(loadedModel, &err, &warn, path);
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

	inline void* getDataPtr(int* bytes, int accessorId, tinygltf::Model* loadedModel)
	{
		// BufferView
		const int bufferViewId = loadedModel->accessors[accessorId].bufferView;
		const tinygltf::BufferView* bufferView = &loadedModel->bufferViews[bufferViewId];
		const int bufferId = bufferView->buffer;
		const int byteLength = bufferView->byteLength;
		const int byteOffset = bufferView->byteOffset;

		// Buffer
		const tinygltf::Buffer* buffer = &loadedModel->buffers[bufferId];

		if (bytes != nullptr)
			*bytes = byteLength;

		return (void*)&buffer->data[byteOffset];
	}

	/*
	  Copies bytes given by accessor and returns pointer to that new memory
	*/
	inline void* copyBufferData(int accessorId, tinygltf::Model* loadedModel)
	{
		// BufferView
		const int bufferViewId = loadedModel->accessors[accessorId].bufferView;
		const tinygltf::BufferView* bufferView = &loadedModel->bufferViews[bufferViewId];
		const int bufferId = bufferView->buffer;
		const int byteLength = bufferView->byteLength;
		const int byteOffset = bufferView->byteOffset;

		// Buffer
		const tinygltf::Buffer* buffer = &loadedModel->buffers[bufferId];

		// Allocate memory for the new buffer
		void* dataPtr = (void*)std::malloc(byteLength);
		assert(dataPtr);

		// Copy data
		std::memcpy(dataPtr, (const void*)&buffer->data[byteOffset], byteLength);

		return dataPtr;
	}

	inline glm::vec3 vec3FromDoubleVector(std::vector<double>* vector)
	{
		return glm::vec3(vector->at(0), vector->at(1), vector->at(2));
	}

	inline glm::quat quatFromDoubleVector(std::vector<double>* vector)
	{
		return glm::quat((float)(vector->at(3)), (float)(vector->at(0)), (float)(vector->at(1)), (float)(vector->at(2)));
	}

	inline glm::mat4 getTRSMatrix(std::vector<double>* translation, std::vector<double>* rotation, std::vector<double>* scale)
	{
		if (translation->empty() && rotation->empty() && scale->empty())
			return glm::mat4{ 1.0f };

		glm::mat4 T{ 1.0f };
		glm::mat4 R{ 1.0f };
		glm::mat4 S{ 1.0f };

		// WARNING: Potentially slow code
		if (!translation->empty())
			T = glm::translate(glm::mat4{ 1.0f }, vec3FromDoubleVector(translation));
		if (!rotation->empty())
			R = glm::mat4_cast(glm::normalize(quatFromDoubleVector(rotation)));
		if (!scale->empty())
			S = glm::scale(glm::mat4{ 1.0f }, vec3FromDoubleVector(scale));

		return T * R * S;
	}

	// glTF matrix alignment: https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#data-alignment
	inline glm::mat4 getMat4FromFloatPtr(float* ptr)
	{
		std::vector<float> m;
		for (size_t i = 0; i < 16; i++)
			m.push_back(*ptr++);

		return glm::mat4{ m.at(0) , m.at(1) , m.at(2) , m.at(3) ,
						  m.at(4) , m.at(5) , m.at(6) , m.at(7) ,
						  m.at(8) , m.at(9) , m.at(10), m.at(11),
						  m.at(12), m.at(13), m.at(14), m.at(15) };
	}
}