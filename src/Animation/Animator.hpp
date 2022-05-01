#pragma once

#include "Animation.hpp"
#include "../Model/SkinnedModel.hpp"


class Animator
{
public:
	Animator();
	Animator(SkinnedModel* const model, Animation* const animation);
	~Animator();

	void setActor(SkinnedModel* const model, Animation* const animation);
	void updateAnimation(float deltaTime);

private:
	SkinnedModel* model	 = nullptr;
	Animation* animation = nullptr;
	float currentTime	 = 0.0f;
	float duration		 = 0.0f;
	int32_t ticksPerSecond	 = 0;
	std::vector<glm::mat4> animChannelMatrices;
	std::vector<glm::mat4> finalBoneMatrices;
	std::map<int32_t, int32_t> armatureToAnimationMap;

	bool checkCompatibility();
};