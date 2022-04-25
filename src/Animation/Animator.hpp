#pragma once

#include "Animation.hpp"
#include "../Model/SkinnedModel.hpp"


class Animator
{
public:
	Animator();
	Animator(SkinnedModel* model, Animation* animation);
	~Animator();

	void setActor(SkinnedModel* model, Animation* animation);
	void updateAnimation(float deltaTime);

private:
	SkinnedModel* model	 = nullptr;
	Animation* animation = nullptr;
	float currentTime	 = 0.0f;
	float duration		 = 0.0f;
	int ticksPerSecond	 = 0;
	std::vector<glm::mat4> animChannelMatrices;
	std::vector<glm::mat4> finalBoneMatrices;
	std::map<int, int> armatureToAnimationMap;
	std::vector<Model*> debugModels;

	bool checkCompatibility();
};