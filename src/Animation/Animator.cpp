#include "Animator.hpp"


Animator::Animator()
{
}

Animator::Animator(SkinnedModel* model, Animation* animation)
{
	setActor(model, animation);
}

Animator::~Animator()
{
}

void Animator::setActor(SkinnedModel* model, Animation* animation)
{
	this->model = model;
	this->animation = animation;
    if (checkCompatibility() != true)
        std::cerr << "ERROR: Animator: Model and animation are incompatible. Please check bone names." << std::endl;

    duration = animation->getDuration();
    ticksPerSecond = animation->getTicksPerSecond();

    animChannelMatrices.clear();
    animChannelMatrices.resize(animation->getChannelCount(), glm::mat4(1.0f));
    finalBoneMatrices.clear();
    finalBoneMatrices.resize(armatureToAnimationMap.size(), glm::mat4(1.0f));
}

void Animator::updateAnimation(float deltaTime)
{
    if (animation)
    {
        currentTime += deltaTime;
        currentTime = fmod(currentTime, duration);
        animation->calculateBoneTransformations(&animChannelMatrices, currentTime, glm::mat4(1.0f), nullptr);
        
        for (size_t i = 0; i < finalBoneMatrices.size(); i++)
        {
            int animChannelId = armatureToAnimationMap[i];
            glm::mat4 ibm = model->getArmature()->at(i).inverseBindMatrix;
            finalBoneMatrices.at(i) = animChannelMatrices.at(animChannelId) * ibm;
        }

        Renderer* renderer = Renderer::getInstance();
        GLProgram* program = renderer->useProgram(Renderer::Program::SKINNED_MESH);
        program->setMatrixArrayUniform("finalBoneMatrices", &finalBoneMatrices);
    }
}

/* Checks if model and animation have the same armature. */
bool Animator::checkCompatibility()
{
    std::vector<Bone>* armature = model->getArmature();
    for (int i = 0; i < armature->size(); i++)
    {
        int id = animation->getChannelIdByName(armature->at(i).name);
        if (id == -1)
            return false;
        armatureToAnimationMap[i] = id;
    }
    return true;
}