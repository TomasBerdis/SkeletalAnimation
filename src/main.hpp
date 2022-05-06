#pragma once

#include "glew.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "GL/GLProgram.hpp"
#include "Camera.hpp"
#include "Model/Model.hpp"
#include "Model/SkinnedModel.hpp"
#include "Animation/Animation.hpp"
#include "Animation/Animator.hpp"

void initialize();
void run();
void cleanup();
void processInput();
void simulate();
void render();
static void glDebugOutputCallback(GLenum source, GLenum type, uint32_t id, GLenum severity, GLsizei length,
    const char *message, const void *userParam);
void initGui();

bool quit = false;
int32_t screenWidth = 1280;
int32_t screenHeight = 768;
SDL_GLContext context;
SDL_Event event;
SDL_Window* window;
Camera* camera;
Model* model;
Model* groundPlane;
Animation* animation;
Animator* animator;


float mouseLastX = screenWidth / 2.0f;
float mouseLastY = screenHeight / 2.0f;
bool firstMouse = true;
bool mouseRightButtonDown = false;
float lastFrameDurationInSec = 0.0f;
uint32_t __measureStartTime__;

//ImGui