#pragma once

#define GLEW_STATIC
#include "GLEW/glew.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "GL/GLProgram.hpp"
#include "Camera.hpp"
#include "Model.hpp"

void initialize();
void run();
void cleanup();
void processInput();
void simulate();
void render();
static void glDebugOutputCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
    const char *message, const void *userParam);

bool quit = false;
int screenWidth = 1280;
int screenHeight = 768;
SDL_Event event;
SDL_Window *window;
Camera *camera;
Model *model;

float mouseLastX = screenWidth / 2.0f;
float mouseLastY = screenHeight / 2.0f;
bool firstMouse = true;
bool mouseRightButtonDown = false;