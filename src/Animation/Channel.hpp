#pragma once

#include <string>
#include <algorithm>

#include "../GltfUtil.hpp"

struct KeyframePosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyframeRotation
{
	glm::quat rotation;
	float timeStamp;
};

struct KeyframeScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Channel
{
public:
	Channel(int nodeId, tinygltf::Node* node);
	~Channel();

	void addChild(Channel* child);
	bool isAnimated();
	int getKeyframePositionId(float animationTime);
	int getKeyframeRotationId(float animationTime);
	int getKeyframeScaleId(float animationTime);
	float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4 getInterpolatedTranslation	(float animationTime);
	glm::mat4 getInterpolatedRotation		(float animationTime);
	glm::mat4 getInterpolatedScale			(float animationTime);
	glm::mat4 getFinalTransformation		(float animationTime);

	unsigned int getId();
	std::string getName();
	glm::mat4 getLocalTransform();
	std::vector<Channel*> getChildren();

	void setKeyframePositions(std::vector<KeyframePosition>* keyframePositions);
	void setKeyframeRotations(std::vector<KeyframeRotation>* keyframeRotations);
	void setKeyframeScales(std::vector<KeyframeScale>* keyframeScales);

private:
	std::string name = "";
	unsigned int id = -1;
	glm::mat4 localTransform;
	glm::vec3 localTranslation;
	glm::quat localRotation;
	glm::vec3 localScale;
	std::vector<KeyframePosition>* keyframePositions = nullptr;
	std::vector<KeyframeRotation>* keyframeRotations = nullptr;
	std::vector<KeyframeScale>* keyframeScales = nullptr;
	std::vector<Channel*> children;
};