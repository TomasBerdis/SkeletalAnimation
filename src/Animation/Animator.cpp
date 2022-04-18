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

    //DEBUG
    /*for (size_t i = 0; i < animChannelMatrices.size(); i++)
    {
        Model* debugModel = new Model(MODEL_SPHERE);
        debugModel->updateProgramType(Renderer::Program::DEBUG);
        debugModels.push_back(debugModel);
    }*/
}

void Animator::updateAnimation(float deltaTime)
{
    if (animation)
    {
        currentTime += ticksPerSecond * deltaTime * 0.1;
        currentTime = fmod(currentTime, duration);
        animation->calculateBoneTransformations(&animChannelMatrices, currentTime, glm::mat4(1.0f), nullptr);
        
        for (size_t i = 0; i < animChannelMatrices.size(); i++)
            finalBoneMatrices.at(i) = animChannelMatrices.at(i) * model->getInverseBindMatrix(i);

        Renderer* renderer = Renderer::getInstance();
        GLProgram* program = renderer->useProgram(Renderer::Program::SKINNED_MESH);
        program->setMatrixArrayUniform("finalBoneMatrices", &finalBoneMatrices);

        model->render();

        //DEBUG
        /*glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        int i = 61;
        debugModels.at(i)->setModelMatrix(animChannelMatrices.at(i));
        debugModels.at(i)->render();
        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
    }
}

/* Checks if model and animation have the same armature. */
bool Animator::checkCompatibility()
{
    return true;
}