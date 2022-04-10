#include "Animation.hpp"

Animation::Animation(std::string path)
{
	gltfUtil::loadFile(path, &loadedAnimation);

	// Load node heirarchy
	int startingNodeId = currentId = loadedAnimation.scenes[loadedAnimation.defaultScene].nodes[0];
	processNode(&loadedAnimation.nodes[startingNodeId], glm::mat4{ 1.0f }, nullptr);

	// Load keyframe data
	loadKeyframes();
}

Animation::~Animation()
{
}

void Animation::processNode(tinygltf::Node* node, glm::mat4 parentTransform, Channel* parent)
{
	glm::mat4 nodeGlobalTransform = parentTransform * gltfUtil::getTRSMatrix(&node->translation, &node->rotation, &node->scale);

	Channel* thisNode = new Channel(currentId, node, nodeGlobalTransform);
	if (parent != nullptr)
		parent->addChild(thisNode);
	channels.push_back(thisNode);

	std::ranges::for_each(node->children, [&](int i)
	{
		currentId = i;
		processNode(&loadedAnimation.nodes[i], nodeGlobalTransform, thisNode);
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

			std::vector<KeyframeRotation>* keyframeRotations = new std::vector<KeyframeRotation>;
			for (unsigned int i = 0; i < count; i++)
			{
				KeyframeRotation rot;
				rot.timeStamp = *timePtr++;
				rot.rotation.w = *rotationPtr++;
				rot.rotation.x = *rotationPtr++;
				rot.rotation.y = *rotationPtr++;
				rot.rotation.z = *rotationPtr++;
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