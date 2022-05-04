#include "Channel.hpp"


Channel::Channel(int32_t nodeId, const tinygltf::Node* const node)
{
	localTransform = gltfUtil::getTRSMatrix(&node->translation, &node->rotation, &node->scale);
	if (node->translation.empty())
		localTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
	else
		localTranslation = gltfUtil::vec3FromDoubleVector(&node->translation);
	if (node->rotation.empty())
		localRotation = glm::quat();
	else
		localRotation = glm::normalize(gltfUtil::quatFromDoubleVector(&node->rotation));
	if (node->scale.empty())
		localScale = glm::vec3(1.0f, 1.0f, 1.0f);
	else
		localScale = gltfUtil::vec3FromDoubleVector(&node->scale);
	name = node->name;
	id = nodeId;
}

Channel::~Channel()
{
}

void Channel::addChild(Channel* child)
{
	children.push_back(child);
}

bool Channel::isAnimated()
{
	bool empty = keyframePositions == nullptr && keyframeRotations == nullptr && keyframeScales == nullptr;
	return !empty;
}

int32_t Channel::getKeyframePositionId(float animationTime)
{
	for (size_t i = 0; i < keyframePositions->size() - 1; i++)
	{
		if (animationTime < keyframePositions->at(i + 1).timeStamp)
			return i;
	}
	return -1;
}

int32_t Channel::getKeyframeRotationId(float animationTime)
{
	for (size_t i = 0; i < keyframeRotations->size() - 1; i++)
	{
		if (animationTime < keyframeRotations->at(i + 1).timeStamp)
			return i;
	}
	return -1;
}

int32_t Channel::getKeyframeScaleId(float animationTime)
{
	for (size_t i = 0; i < keyframeScales->size() - 1; i++)
	{
		if (animationTime < keyframeScales->at(i + 1).timeStamp)
			return i;
	}
	return -1;
}

float Channel::getScaleFactor(const float lastTimeStamp, const float nextTimeStamp, const float animationTime)
{
	const float midWayLength	= animationTime - lastTimeStamp;
	const float framesDiff		= nextTimeStamp - lastTimeStamp;
	return midWayLength / framesDiff;
}

glm::mat4 Channel::getInterpolatedTranslation(float animationTime)
{
	if (keyframePositions->size() == 1)
		return glm::translate(glm::mat4(1.0f), keyframePositions->at(0).position);

	const int32_t lastKeyframeId = getKeyframePositionId(animationTime);
	const int32_t nextKeyframeId = lastKeyframeId + 1;
	const float scaleFactor		 = getScaleFactor(keyframePositions->at(lastKeyframeId).timeStamp,
									keyframePositions->at(nextKeyframeId).timeStamp, animationTime);

	glm::vec3 finalPosition		 = glm::mix(keyframePositions->at(lastKeyframeId).position,
									keyframePositions->at(nextKeyframeId).position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Channel::getInterpolatedRotation(float animationTime)
{
	if (keyframeRotations->size() == 1)
	{
		auto rotation = glm::normalize(keyframeRotations->at(0).rotation);
		return glm::mat4_cast(rotation);
	}

	const int32_t lastKeyframeId = getKeyframeRotationId(animationTime);
	const int32_t nextKeyframeId = lastKeyframeId + 1;
	const float scaleFactor		 = getScaleFactor(keyframeRotations->at(lastKeyframeId).timeStamp,
									keyframeRotations->at(nextKeyframeId).timeStamp, animationTime);

	glm::quat finalRotation		 = glm::slerp(keyframeRotations->at(lastKeyframeId).rotation,
									keyframeRotations->at(nextKeyframeId).rotation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::mat4_cast(finalRotation);
}

glm::mat4 Channel::getInterpolatedScale(float animationTime)
{
	if (keyframeScales->size() == 1)
		return glm::scale(glm::mat4(1.0f), keyframeScales->at(0).scale);

	const int32_t lastKeyframeId = getKeyframeScaleId(animationTime);
	const int32_t nextKeyframeId = lastKeyframeId + 1;
	const float scaleFactor		 = getScaleFactor(keyframeScales->at(lastKeyframeId).timeStamp,
									keyframeScales->at(nextKeyframeId).timeStamp, animationTime);

	const glm::vec3 finalScale	 = glm::mix(keyframeScales->at(lastKeyframeId).scale,
									keyframeScales->at(nextKeyframeId).scale, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}

glm::mat4 Channel::getFinalTransformation(float animationTime)
{
	glm::mat4 translation	= glm::mat4(1.0f);
	glm::mat4 rotation		= glm::mat4(1.0f);
	glm::mat4 scale			= glm::mat4(1.0f);
	if (keyframePositions)
		translation = getInterpolatedTranslation(animationTime);
	else
		translation = glm::translate(glm::mat4(1.0f), localTranslation);
	if (keyframeRotations)
		rotation	= getInterpolatedRotation(animationTime);
	else
		rotation	= glm::mat4_cast(localRotation);
	if (keyframeScales)
		scale		= getInterpolatedScale(animationTime);
	else
		scale		= glm::scale(glm::mat4(1.0f), localScale);

	return (translation * rotation * scale);
}

uint32_t Channel::getId()
{
	return id;
}

std::string Channel::getName()
{
	return name;
}

glm::mat4 Channel::getLocalTransform()
{
	return localTransform;
}

std::vector<Channel*> Channel::getChildren()
{
	return children;
}

void Channel::setKeyframePositions(std::vector<KeyframePosition>* keyframePositions)
{
	this->keyframePositions = keyframePositions;
}

void Channel::setKeyframeRotations(std::vector<KeyframeRotation>* keyframeRotations)
{
	this->keyframeRotations = keyframeRotations;
}

void Channel::setKeyframeScales(std::vector<KeyframeScale>* keyframeScales)
{
	this->keyframeScales = keyframeScales;
}