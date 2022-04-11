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

	void calculateDuration();
	void calculateTicksPerSecond();

	float getDuration();
	int getTicksPerSecond();
	unsigned int getChannelCount();
	void calculateBoneTransformations(std::vector<glm::mat4>* boneMatrices, float animationTime, glm::mat4 parentTransform, Channel* node);

private:
	tinygltf::Model loadedAnimation;
	float duration = 0.0f;
	int ticksPerSecond = 0;
	std::vector<Channel*> channels;
	int currentId = -1;

	void processNode(tinygltf::Node* node, glm::mat4 parentTransform, Channel* parent);
	void loadKeyframes();
};