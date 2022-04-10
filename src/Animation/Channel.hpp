#pragma once

#include <string>

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
	Channel(int nodeId, tinygltf::Node* node, glm::mat4 parentTransform);
	~Channel();

	void addChild(Channel* child);
	unsigned int getId();
	void setKeyframePositions(std::vector<KeyframePosition>* keyframePositions);
	void setKeyframeRotations(std::vector<KeyframeRotation>* keyframeRotations);
	void setKeyframeScales(std::vector<KeyframeScale>* keyframeScales);

private:
	std::string name = "";
	unsigned int id = 0;
	glm::mat4 globalTransform;
	std::vector<KeyframePosition>* keyframePositions;
	std::vector<KeyframeRotation>* keyframeRotations;
	std::vector<KeyframeScale>* keyframeScales;
	std::vector<Channel*> children;
};