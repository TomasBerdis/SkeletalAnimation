#pragma once

#include "GL/GLProgram.hpp"
#include "Camera.hpp"

#include <map>
#include <iostream>


/* Singleton */
class Renderer
{
public:
	enum Program
	{
		MESH, SKINNED_MESH
	};

	static Renderer *getInstance();
	GLProgram *useProgram(Program program);
	Camera *getCamera();
	void setCamera(Camera* camera);


private:
	static Renderer *instance;
	Camera *camera = nullptr;
	std::map<Program, GLProgram*> programMap;


	Renderer();
	~Renderer();
	void createProgram(Program programId);
};