#include "Animation.hpp"

Animation::Animation(std::string path)
{
	gltfUtil::loadFile(path, &loadedAnimation);
	name = loadedAnimation.animations[0].name;

	// WARNING: Assuming a lot of things here
	timeAccessor = loadedAnimation.animations[0].samplers[0].input;
	calculateDuration();
	calculateTicksPerSecond();

	// Load node heirarchy
	int startingNodeId = currentId = loadedAnimation.scenes[loadedAnimation.defaultScene].nodes[0];
	processNode(&loadedAnimation.nodes[startingNodeId], nullptr);

	// Load keyframe data
	loadKeyframes();
}
 
Animation::~Animation()
{
}

void Animation::calculateDuration()
{
	duration = std::ranges::max(loadedAnimation.accessors[timeAccessor].maxValues);
}

void Animation::calculateTicksPerSecond()
{
	int bytes;
	float* timePtr = (float*) gltfUtil::getDataPtr(&bytes, timeAccessor, &loadedAnimation);
	int frames = bytes / sizeof(float);
	ticksPerSecond = frames / duration;
}

float Animation::getDuration()
{
	return duration;
}

int Animation::getTicksPerSecond()
{
	return ticksPerSecond;
}

Channel* Animation::getChannel(int id)
{
	return channels[id];
}

Channel* Animation::getChannelById(int id)
{
	if (auto result = std::ranges::find_if(channels, [&](Channel* ch) { return ch->getId() == id; }); result != channels.end())
		return result[0];
	else
		return nullptr;
}

unsigned int Animation::getChannelCount()
{
	return channels.size();
}

int Animation::getChannelIdByName(std::string name)
{
	if (auto result = std::ranges::find_if(channels, [&](Channel* ch) { return ch->getName() == name; }); result != channels.end())
		return result[0]->getId();
	else
		return -1;
}

void Animation::processNode(tinygltf::Node* node, Channel* parent)
{
	Channel* thisNode = new Channel(currentId, node);
	if (parent != nullptr)
		parent->addChild(thisNode);
	channels.push_back(thisNode);

	std::ranges::for_each(node->children, [&](int i)
	{
		currentId = i;
		processNode(&loadedAnimation.nodes[i], thisNode);
	});
}

void Animation::loadKeyframes()
{
	std::ranges::for_each(loadedAnimation.animations[0].channels, [&](tinygltf::AnimationChannel animChannel)
	{
		tinygltf::AnimationSampler* sampler = &loadedAnimation.animations[0].samplers[animChannel.sampler];
		int const timeAccessorId = sampler->input;
		int const valueAccessorId = sampler->output;
		auto channel = std::ranges::find_if(channels, [&](Channel* ch) { return ch->getId() == animChannel.target_node; });

		if (animChannel.target_path == "translation")
		{
			int bytes;
			float* timePtr			= (float*) gltfUtil::getDataPtr(NULL, timeAccessorId, &loadedAnimation);
			float* translationPtr	= (float*) gltfUtil::getDataPtr(&bytes, valueAccessorId, &loadedAnimation);
			// calculate how many keyframes to load
			int count = (bytes / sizeof(float)) / 3;	// number of struct elements = 3

			std::vector<KeyframePosition>* keyframePositions = new std::vector<KeyframePosition>;
			for (unsigned int i = 0; i < count; i++)
			{
				KeyframePosition pos;
				pos.timeStamp = *timePtr++;
				pos.position.x = *translationPtr++;
				pos.position.y = *translationPtr++;
				pos.position.z = *translationPtr++;
				keyframePositions->push_back(pos);
			}
			channel[0]->setKeyframePositions(keyframePositions);
		}
		else if (animChannel.target_path == "rotation")
		{
			int bytes;
			float* timePtr		= (float*) gltfUtil::getDataPtr(NULL, timeAccessorId, &loadedAnimation);
			float* rotationPtr	= (float*) gltfUtil::getDataPtr(&bytes, valueAccessorId, &loadedAnimation);
			// calculate how many keyframes to load
			int count = (bytes / sizeof(float)) / 4;	// number of struct elements = 4

			// Quaternion layout: https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#transformations
			std::vector<KeyframeRotation>* keyframeRotations = new std::vector<KeyframeRotation>;
			for (unsigned int i = 0; i < count; i++)
			{
				KeyframeRotation rot;
				rot.timeStamp = *timePtr++;
				rot.rotation.x = *rotationPtr++;
				rot.rotation.y = *rotationPtr++;
				rot.rotation.z = *rotationPtr++;
				rot.rotation.w = *rotationPtr++;
				keyframeRotations->push_back(rot);
			}
			channel[0]->setKeyframeRotations(keyframeRotations);
		}
		else if (animChannel.target_path == "scale")
		{
			int bytes;
			float* timePtr			= (float*) gltfUtil::getDataPtr(NULL, timeAccessorId, &loadedAnimation);
			float* scalingPtr		= (float*) gltfUtil::getDataPtr(&bytes, valueAccessorId, &loadedAnimation);
			// calculate how many keyframes to load
			int count = (bytes / sizeof(float)) / 3;	// number of struct elements = 3

			std::vector<KeyframeScale>* keyframeScales = new std::vector<KeyframeScale>;
			for (unsigned int i = 0; i < count; i++)
			{
				KeyframeScale scale;
				scale.timeStamp = *timePtr++;
				scale.scale.x = *scalingPtr++;
				scale.scale.y = *scalingPtr++;
				scale.scale.z = *scalingPtr++;
				keyframeScales->push_back(scale);
			}
			channel[0]->setKeyframeScales(keyframeScales);
		}
	});
}

void Animation::calculateBoneTransformations(std::vector<glm::mat4>* boneMatrices, float animationTime,
	glm::mat4 parentTransform, Channel* node)
{
	if (node == nullptr)
		node = channels[0];

	glm::mat4 globalTransform;

	if (!node->isAnimated())
		globalTransform = parentTransform * node->getLocalTransform();
	else
		globalTransform = parentTransform * node->getFinalTransformation(animationTime);

	boneMatrices->at(node->getId()) = globalTransform;

	std::ranges::for_each(node->getChildren(), [&](Channel* c)
	{
		calculateBoneTransformations(boneMatrices, animationTime, globalTransform, c);
	});
}