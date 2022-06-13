#include "main.hpp"

// tinyGLTF
// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

#define MEASURE(code)   __measureStartTime__ = SDL_GetTicks(); \
std::cout << "###MEASURE_START:\nFILE:" << __FILE__ << '\n' << "LINE: " << __LINE__ << '\n'; \
code std::cout << "Execution time: " << (SDL_GetTicks() - __measureStartTime__) << "ms\n###MEASURE_END\n";

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

int32_t main(int32_t argc, char **argv)
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
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // OpenGL
#ifdef DEBUG_BUILD
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // DEBUG_BUILD
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    context = SDL_GL_CreateContext(window);
    if (context == nullptr)
        fprintf(stderr, "Failed to initialize OpenGL context.\n");
    printf("%s\n", glGetString(GL_VERSION));

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
        fprintf(stderr, "Failed to initialize GLEW.\n");

    int32_t contextFlags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        std::cout << "Debug context created." << '\n';
        // Comment out to toggle debug output
        //glEnable(GL_DEBUG_OUTPUT);
        //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        //glDebugMessageCallback(glDebugOutputCallback, nullptr);
        //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 430 core");

    // Camera
    camera = new Camera(glm::vec3(0.0f, 2.0f, 3.0f), 45, (float) screenWidth / (float) screenHeight, 0.1f, 1000.0f);
    camera->rotateCamera(900.0f, -270.0f);	// reset rotation

    // Renderer singleton
    Renderer *renderer = Renderer::getInstance();
    renderer->setCamera(camera);

    groundPlane  = std::make_shared<Model>(R"(../res/models/plane.glb)");
    groundPlane->scale({ 5.f, 5.f, 5.f });
    groundPlane->rotate(glm::quat(0.7071f, 0.7071f, 0.0f, 0.0f));
    groundPlane->updateProgramType(Renderer::Program::DEBUG);

    // Load "../res/" directory models and animations
    for (const auto& file : std::filesystem::directory_iterator("../res/models"))
    {
        if (!std::filesystem::is_directory(file))
            parseFile(file.path().string());
    }
    for (const auto& file : std::filesystem::directory_iterator("../res/animations"))
    {
        if (!std::filesystem::is_directory(file))
            parseFile(file.path().string());
    }

    selectedModelName       = models.begin()->first;
    selectedAnimationName   = animations.begin()->first;
    model = models.begin()->second;
    animator = new Animator((SkinnedModel*) model.get(), animations.begin()->second.get());
}

void run()
{
    // Game loop
    while (!quit)
    {
        uint32_t startTime = SDL_GetTicks();

        processInput();
        simulate();
        render();

        uint32_t currTime = SDL_GetTicks();
        lastFrameDurationInSec = (currTime - startTime) / 1000.0f;
    }
}

void cleanup()
{
    /*for (const auto & i : models)
        i.second->~Model();
    for (const auto& i : animations)
        i.second->~Animation();*/

    delete animator;
    delete camera;

    models.clear();
    animations.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void processInput()
{
    float speed = 0.05f;

    while (SDL_PollEvent(&event) > 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
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
    // animation updates
    animator->updateAnimation(lastFrameDurationInSec);
}

void render()
{
    //ImGui
    initGui();

    glClearColor(0.055, 0.18, 0.216, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render calls
    model->render();
    groundPlane->render();

    //ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}

static void glDebugOutputCallback(GLenum source, GLenum type, uint32_t id, GLenum severity, GLsizei length,
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

void initGui()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            std::cout << "filePathName: " << filePathName << '\n';
            std::cout << "filePath: " << filePath << '\n';
            parseFile(filePathName);
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }

    {
        ImGui::Text("Models:");
        if (ImGui::BeginListBox("##listbox 1", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (const auto &item : models)
            {
                const bool isSelected = (selectedModelName == item.first.c_str());
                if (ImGui::Selectable(item.first.c_str(), isSelected))
                    selectedModelName = item.first;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        ImGui::Text("Animations:");
        if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (const auto& item : animations)
            {
                const bool isSelected = (selectedAnimationName == item.first.c_str());
                if (ImGui::Selectable(item.first.c_str(), isSelected))
                    selectedAnimationName = item.first;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        if (ImGui::Button("Change"))
        {
            model = models[selectedModelName];
            animator->setActor((SkinnedModel*) model.get(), animations[selectedAnimationName].get());
        }

        // open Dialog Simple
        if (ImGui::Button("Import files..."))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".glb,.gltf", "../res/");
    }
}

void parseFile(std::string path)
{
    // Load file
    tinygltf::Model file;
    gltfUtil::loadFile(path, &file);

    const int32_t backSlashPos = path.find_last_of("\\") + 1;
    const int32_t forwSlashPos = path.find_last_of("/") + 1;
    const int32_t pos = std::max(backSlashPos, forwSlashPos);
    std::string fileName = path.substr(pos, path.length() - pos);

    // Determine contents of the file
    bool hasMeshes = false;
    bool hasSkin = false;
    bool hasAnimations = false;

    if (!file.meshes.empty())
        hasMeshes = true;

    if (!file.skins.empty())
        hasSkin = true;

    if (!file.animations.empty())
        hasAnimations = true;

    // Parse data
    if (hasMeshes && hasSkin)
    {
        std::shared_ptr<Model> model = std::make_shared<SkinnedModel>(&file);
        models[fileName] = model;
    }
    else if (hasMeshes)
    {
        std::shared_ptr<Model> model = std::make_shared<Model>(&file);
        models[fileName] = model;
    }

    if (hasAnimations)
    {
        for (size_t i = 0; i < file.animations.size(); i++)
        {
            std::shared_ptr<Animation> animation = std::make_shared<Animation>(&file, i);
            animations[fileName + " || " + file.animations[i].name] = animation;
        }
    }
}