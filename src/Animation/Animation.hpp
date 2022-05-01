#pragma once

#include "../GltfUtil.hpp"
#include "Channel.hpp"
/*DEBUG*/ #include "../Model/Model.hpp"

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
	int32_t getTicksPerSecond();
	Channel* getChannel(int32_t id);
	Channel* getChannelById(int32_t id);
	uint32_t getChannelCount();
	int32_t getChannelIdByName(std::string name);
	void calculateBoneTransformations(std::vector<glm::mat4>* boneMatrices, const float animationTime, const glm::mat4 parentTransform, Channel* node);

private:
	tinygltf::Model loadedAnimation;
	std::string name;
	float duration = 0.0f;
	int32_t ticksPerSecond = 0;
	std::vector<Channel*> channels;
	int32_t currentId = -1;
	int32_t timeAccessor = -1;

	void processNode(tinygltf::Node* node, Channel* parent);
	void loadKeyframes();
};