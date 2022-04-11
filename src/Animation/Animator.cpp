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
    checkCompatibility();

    duration = animation->getDuration();
    ticksPerSecond = animation->getTicksPerSecond();

    animChannelMatrices.clear();
    animChannelMatrices.resize(animation->getChannelCount(), glm::mat4 { 1.0f });
    finalBoneMatrices.clear();
    finalBoneMatrices.resize(animation->getChannelCount(), glm::mat4 { 1.0f });
}

void Animator::updateAnimation(float deltaTime)
{
    if (animation)
    {
        currentTime += ticksPerSecond * deltaTime * 0.001;
        currentTime = fmod(currentTime, duration);
        animation->calculateBoneTransformations(&animChannelMatrices, currentTime, glm::mat4(1.0f), nullptr);
        
        for (size_t i = 0; i < animChannelMatrices.size(); i++)
            finalBoneMatrices.at(i) = animChannelMatrices.at(i) * model->getInverseBindMatrix(i);

        Renderer* renderer = Renderer::getInstance();
        GLProgram* program = renderer->useProgram(Renderer::Program::SKINNED_MESH);
        program->setMatrixArrayUniform("finalBoneMatrices", &finalBoneMatrices);

        model->render();
    }
}

/* Checks if model and animation have the same armature. */
bool Animator::checkCompatibility()
{
    return true;
}