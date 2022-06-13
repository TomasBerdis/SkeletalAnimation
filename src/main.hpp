#pragma once

#include "glew.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"

#include "GL/GLProgram.hpp"
#include "Camera.hpp"
#include "Model/Model.hpp"
#include "Model/SkinnedModel.hpp"
#include "Animation/Animation.hpp"
#include "Animation/Animator.hpp"

static void initialize();
static void run();
static void cleanup();
static void processInput();
static void simulate();
static void render();
static void glDebugOutputCallback(GLenum source, GLenum type, uint32_t id, GLenum severity, GLsizei length,
    const char *message, const void *userParam);
static void initGui();
static void parseFile(std::string path);

static bool quit = false;
static int32_t screenWidth = 1280;
static int32_t screenHeight = 768;
static SDL_GLContext context;
static SDL_Event event;
static SDL_Window* window;
static Camera* camera;
static std::shared_ptr<Model> model;
static std::shared_ptr<Model> groundPlane;
static std::shared_ptr<Animation> animation;
static Animator* animator;
static std::unordered_map<std::string, std::shared_ptr<Model>> models;
static std::unordered_map<std::string, std::shared_ptr<Animation>> animations;
static std::string selectedModelName;
static std::string selectedAnimationName;

static float mouseLastX = screenWidth / 2.0f;
static float mouseLastY = screenHeight / 2.0f;
static bool firstMouse = true;
static bool mouseRightButtonDown = false;
static float lastFrameDurationInSec = 0.0f;
static uint32_t __measureStartTime__;