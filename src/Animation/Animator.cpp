#include "Animator.hpp"


Animator::Animator()
{
}

Animator::Animator(SkinnedModel* const model, Animation* const animation)
{
	setActor(model, animation);
}

Animator::~Animator()
{
    model = nullptr;
    animation = nullptr;
}

void Animator::setActor(SkinnedModel* const model, Animation* const animation)
{
    if (this->model == model && this->animation == animation)
        return;

	this->model = model;
	this->animation = animation;
    if (checkCompatibility() != true)
        std::cerr << "ERROR: Animator: Model and animation are incompatible. Please check bone names." << '\n';

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
            const int32_t animChannelId = armatureToAnimationMap[i];
            glm::mat4 ibm = model->getArmature()->at(i).inverseBindMatrix;
            finalBoneMatrices.at(i) = animChannelMatrices.at(animChannelId) * ibm;
        }

        Renderer* renderer = Renderer::getInstance();
        GLProgram* program = renderer->useProgram(Renderer::Program::SKINNED_MESH);

        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(finalBoneMatrices.at(0)) * finalBoneMatrices.size(), finalBoneMatrices.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}

/* Checks if model and animation have the same armature. */
bool Animator::checkCompatibility()
{
    std::vector<Bone>* armature = model->getArmature();
    armatureToAnimationMap.clear();
    for (size_t i = 0; i < armature->size(); i++)
    {
        const int32_t id = animation->getChannelIdByName(armature->at(i).name);
        if (id == -1)
            return false;
        armatureToAnimationMap[i] = id;
    }
    return true;
}