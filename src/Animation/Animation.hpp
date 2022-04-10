#pragma once

#include "../GltfUtil.hpp"
#include "Channel.hpp"

#include <string>
#include <algorithm>

class Animation
{
public:
	Animation(std::string path);
	~Animation();


private:
	tinygltf::Model loadedAnimation;
	float duration = 0.0f;
	int ticksPerSecond = 0;
	std::vector<Channel*> channels;
	int currentId = -1;

	void processNode(tinygltf::Node* node, glm::mat4 parentTransform, Channel* parent);
	void loadKeyframes();
};