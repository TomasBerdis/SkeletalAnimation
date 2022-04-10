#include "Channel.hpp"


Channel::Channel(int nodeId, tinygltf::Node* node, glm::mat4 parentTransform)
{
	globalTransform = parentTransform;
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

unsigned int Channel::getId()
{
	return id;
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