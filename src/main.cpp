#include "main.hpp"

// tinyGLTF
// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tinyGLTF/tiny_gltf.h"

 
int main(int argc, char **argv)
{
    initialize();
    run();
    cleanup();

    return 0;
}

void initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());

    window = SDL_CreateWindow("Skeletal Animation demo",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

    // OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    printf("%s\n", glGetString(GL_VERSION));

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
        fprintf(stderr, "Failed to initialize GLEW\n");

    int contextFlags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        std::cout << "Debug context created." << std::endl;
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutputCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }

    glEnable(GL_DEPTH_TEST);

    // Camera
    camera = new Camera(glm::vec3(0.0f, 2.0f, 3.0f), 45, (float) screenWidth / (float) screenHeight, 0.1f, 1000.0f);
    camera->rotateCamera(900.0f, -270.0f);	// reset rotation

    // Renderer singleton
    Renderer *renderer = Renderer::getInstance();
    renderer->setCamera(camera);

    model       = new SkinnedModel(MODEL_ALEX);
    animation   = new Animation(ANIMATION_LEG_SWEEP);
    animator    = new Animator((SkinnedModel*) model, animation);
}

void run()
{
    // Game loop
    while (!quit)
    {
        processInput();
        simulate();
        render();
    }
}

void cleanup()
{
    SDL_Quit();
}

void processInput()
{
    float speed = 0.05f;

    while (SDL_PollEvent(&event) > 0)
    {
        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_UP:
                        camera->increaseFov(5);
                        printf("FOV INCREASED\n");
                        break;
                    case SDLK_DOWN:
                        camera->decreaseFov(5);
                        printf("FOV DECREASED\n");
                        break;
                    case SDLK_a:
                        camera->moveCamera(Camera::Direction::LEFT, speed);
                        break;
                    case SDLK_d:
                        camera->moveCamera(Camera::Direction::RIGHT, speed);
                        break;
                    case SDLK_w:
                        camera->moveCamera(Camera::Direction::FORWARDS, speed);
                        break;
                    case SDLK_s:
                        camera->moveCamera(Camera::Direction::BACKWARDS, speed);
                        break;
                    case SDLK_q:
                        camera->moveCamera(Camera::Direction::UP, speed);
                        break;
                    case SDLK_e:
                        camera->moveCamera(Camera::Direction::DOWN, speed);
                        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        break;
                    case SDL_BUTTON_RIGHT:
                        mouseRightButtonDown = true;
                        mouseLastX = event.button.x;
                        mouseLastY = event.button.y;
                        break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        break;
                    case SDL_BUTTON_RIGHT:
                        mouseRightButtonDown = false;
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (mouseRightButtonDown)
                {
                    float xpos = static_cast<float>(event.motion.x);
                    float ypos = static_cast<float>(event.motion.y);

                    if (firstMouse)
                    {
                        mouseLastX = xpos;
                        mouseLastY = ypos;
                        firstMouse = false;
                    }

                    float xoffset = xpos - mouseLastX;
                    float yoffset = mouseLastY - ypos; // reversed since y-coordinates go from bottom to top

                    mouseLastX = xpos;
                    mouseLastY = ypos;

                    camera->rotateCamera(-xoffset, yoffset);
                }
                break;
        }
    }
}

void simulate()
{

}

void render()
{
    glClearColor(1.0f, 1.0f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render calls
    animator->updateAnimation(0.1f);

    SDL_GL_SwapWindow(window);
}

static void APIENTRY glDebugOutputCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
    const char *message, const void *userParam)
{
    printf("Debug message: id %d, %s \n", id, message);

    printf("Message source: ");
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:               printf("API\n");                break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     printf("Window System\n");      break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   printf("Shader Compiler\n");    break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:       printf("Third Party\n");        break;
        case GL_DEBUG_SOURCE_APPLICATION:       printf("Application \n");       break;
        case GL_DEBUG_SOURCE_OTHER:             printf("Other\n");              break;
    }

    printf("Error type: ");
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               printf("Error\n");                  break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: printf("Deprecated Behaviour\n");   break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  printf("Undefined Behaviour \n");   break;
        case GL_DEBUG_TYPE_PORTABILITY:         printf("Portability\n");            break;
        case GL_DEBUG_TYPE_PERFORMANCE:         printf("Pentormance\n");            break;
        case GL_DEBUG_TYPE_MARKER:              printf("Marker\n");                 break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          printf("Push Group\n");             break;
        case GL_DEBUG_TYPE_POP_GROUP:           printf("Pop Group\n");              break;
        case GL_DEBUG_TYPE_OTHER:               printf("Other\n");                  break;
    }

    printf("Severity: ");
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         printf("High\n");          break;
        case GL_DEBUG_SEVERITY_MEDIUM:       printf("Medium\n");        break;
        case GL_DEBUG_SEVERITY_LOW:          printf("Low\n");           break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: printf("Notification\n");  break;
    }
}