#pragma once

#include "GL/GLProgram.hpp"
#include "Camera.hpp"

#include <map>
#include <iostream>
#include "tinyGLTF/tiny_gltf.h"

struct Texture
{
	GLuint id = 0;
	int width;
	int height;
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
		MESH, SKINNED_MESH
	};

	static Renderer *getInstance();
	GLProgram *useProgram(Program program);
	Camera *getCamera();
	void setCamera(Camera* camera);
	void loadTexture(tinygltf::Image* image);
	void bindTexture(std::string name, unsigned int textureUnit);


private:
	static Renderer *instance;
	Camera *camera = nullptr;
	std::map<Program, GLProgram*> programMap;
	std::map<std::string, Texture> textureMap;


	Renderer();
	~Renderer();
	void createProgram(Program programId);
};