#include "Renderer.hpp"

Renderer *Renderer::instance = nullptr;
const std::string Renderer::debugTexture = "debugTexture.png";

Renderer::Renderer()
{
	programMap.clear();
	loadTexture(DEBUG_TEXTURE);
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
		case Renderer::Program::MESH:
			program = new GLProgram({ MESH_VS, MESH_FS });
			break;
		case Renderer::Program::SKINNED_MESH:
			program = new GLProgram({ SKINNED_MESH_VS, SKINNED_MESH_FS });
			break;
		case Renderer::Program::DEBUG:
			program = new GLProgram({ DEBUG_VS, DEBUG_FS });
			break;
		default:
			std::cerr << "Renderer: Cannot create unexisting program" << '\n';
	}

	programMap.insert({ programId, program });
}

void Renderer::loadTexture(const std::string path)
{
	const int32_t pos = path.find_last_of("/") + 1;
	std::string name = path.substr(pos, path.length() - pos);

	if (textureMap.contains(name))
		return;

	Texture texture;
	int32_t channels;
	texture.data = stbi_load(path.c_str(), &texture.width, &texture.height, &channels, 0);

	if (texture.data == NULL)
		std::cerr << "Renderer: Cannot load texture: " << name << '\n';

	if (channels == 3)
		texture.format = GL_RGB;
	else if (channels == 4)
		texture.format = GL_RGBA;
	else
		std::cerr << "Renderer: Unsupported texture format" << '\n';

	texture.type = GL_UNSIGNED_BYTE;

	// OpenGL code
	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0,
		texture.format, texture.type, texture.data);

	textureMap.insert({ name, texture });

	std::cout << "Texture: \'" << name << "\' loaded" << '\n';

	stbi_image_free(texture.data);
}

void Renderer::loadTexture(tinygltf::Image* image)
{
	if (textureMap.contains(image->name))
		return;

	Texture texture;
	texture.width = image->width;
	texture.height = image->height;

	texture.format = GL_RGBA;
	if (image->component == 1)
		texture.format = GL_RED;
	else if (image->component == 2)
		texture.format = GL_RG;
	else if (image->component == 3)
		texture.format = GL_RGB;

	texture.type = GL_UNSIGNED_BYTE;
	if (image->bits == 16)
		texture.type = GL_UNSIGNED_SHORT;

	if (texture.type == GL_UNSIGNED_BYTE)
	{
		texture.data = malloc(sizeof(unsigned char) * image->image.size());
		memcpy(texture.data, (const void*) &image->image.at(0), sizeof(unsigned char) * image->image.size());
	}
	else if (texture.type == GL_UNSIGNED_SHORT)
	{
		texture.data = malloc(sizeof(unsigned short) * image->image.size());
		memcpy(texture.data, (const void*) &image->image.at(0), sizeof(unsigned short) * image->image.size());
	}

	// OpenGL code
	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0,
		texture.format, texture.type, texture.data);

	textureMap.insert({ image->name, texture });

	std::cout << "Texture: \'" << image->name << "\' loaded" << '\n';
}

void Renderer::bindTexture(const std::string name, uint32_t textureUnit)
{
	assert(textureMap.contains(name));
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureMap[name].id);
}