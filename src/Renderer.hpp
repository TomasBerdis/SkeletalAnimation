#pragma once

#include "GL/GLProgram.hpp"
#include "Camera.hpp"

#include <map>
#include <iostream>
#include "tinyGLTF/tiny_gltf.h"
#include "tinyGLTF/stb_image.h"

struct Texture
{
	GLuint id = 0;
	int32_t width;
	int32_t height;
	GLenum format;
	GLenum type;
	void* data;
};

/* Singleton */
class Renderer
{
public:
	enum class Program
	{
		MESH, SKINNED_MESH, DEBUG
	};
	static const std::string debugTexture;

	static Renderer *getInstance();
	GLProgram *useProgram(Program program);
	Camera *getCamera();
	void setCamera(Camera* camera);
	void loadTexture(const std::string name);
	void loadTexture(const tinygltf::Image* const image);
	void bindTexture(const std::string name, uint32_t textureUnit);


private:
	static Renderer *instance;
	Camera *camera = nullptr;
	std::unordered_map<Program, GLProgram*> programMap;
	std::unordered_map<std::string, Texture> textureMap;


	Renderer();
	~Renderer();
	void createProgram(Program programId);
};