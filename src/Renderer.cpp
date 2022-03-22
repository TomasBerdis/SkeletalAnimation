#include "Renderer.hpp"

Renderer *Renderer::instance = nullptr;

Renderer::Renderer()
{
	programMap.clear();
}

Renderer::~Renderer()
{
}

Renderer* Renderer::getInstance()
{
	if (instance == nullptr)
		instance = new Renderer;
	return instance;
}

GLProgram* Renderer::useProgram(Program program)
{
	GLProgram* glProgram = nullptr;

	if (!programMap.contains(program))
		createProgram(program);

	glProgram = programMap[program];

	glProgram->use();

	return glProgram;
}

Camera *Renderer::getCamera()
{
	assert(camera);
	return camera;
}

void Renderer::setCamera(Camera *camera)
{
	this->camera = camera;
}

void Renderer::createProgram(Program programId)
{
	GLProgram* program;

	switch (programId)
	{
	case Renderer::MESH:
		program = new GLProgram({ MESH_VS, MESH_FS });
	case Renderer::SKINNED_MESH:
		break;
	default:
		std::cerr << "Renderer: Cannot create unexisting program" << std::endl;
	}

	programMap.insert({ programId, program });
}
