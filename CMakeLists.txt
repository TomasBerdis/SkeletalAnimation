cmake_minimum_required(VERSION 3.18)
project(SkeletalAnimation)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

# GLOB_RECURSE for recursive search
FILE(GLOB_RECURSE cppSources src/*.cpp)
FILE(GLOB_RECURSE hppSources src/*.hpp)
set(sources
    ${cppSources}
    ${hppSources}
)

add_executable(${PROJECT_NAME} ${sources})
add_subdirectory(vendor/glew)
add_subdirectory(vendor/SDL2)
add_library(imgui
    # Main Imgui files
    vendor/imgui/imgui.cpp vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp vendor/imgui/imgui_widgets.cpp
    # SDL2+OpenGL-specific files
    vendor/imgui/backends/imgui_impl_sdl.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp)
# Make SDL2 available to Imgui
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/SDL2/include)
# imgui/backends/ contains the SDL implementation
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/ ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/backends/)
# Configure Imgui to use OpengGL through SDL2
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL_opengl.h>  GL_GLEXT_PROTOTYPES=1)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/glew/include/GL
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/glm/include
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/SDL2/include
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/tinyGLTF/include
)
target_link_libraries(${PROJECT_NAME}
    libglew_static
    SDL2main
    SDL2-static
    opengl32.lib
    imgui
)

# shaders
find_file(debugVS debugVS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(debugFS debugFS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(meshVS meshVS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(meshFS meshFS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(skinnedMeshVS skinnedMeshVS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(skinnedMeshFS skinnedMeshFS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)

# images
find_file(debugTexture debugTexture.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/images
)

# models
find_file(alex alex.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
find_file(gasmask gasmask.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
find_file(kachujin kachujin.gltf
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
find_file(maw maw.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
find_file(ninja ninja.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
find_file(paladin paladin.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
find_file(vampire vampire.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
find_file(whale whaleBonedSized.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
#simple models
find_file(cube cube.glb
HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
find_file(plane plane.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)
find_file(sphere sphere.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/models
)

# animations
find_file(crawling crawling.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/animations
)
find_file(dying dying.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/animations
)
find_file(leg_sweep leg_sweep.gltf
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/animations
)
find_file(running running.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/animations
)
find_file(sitting_clap sitting_clap.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/animations
)
find_file(swing_to_land swing_to_land.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/animations
)
find_file(taunt taunt.glb
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/animations
)

target_compile_definitions(${PROJECT_NAME} PUBLIC "DEBUG_VS=\"${debugVS}\"" "DEBUG_FS=\"${debugFS}\"" "MESH_VS=\"${meshVS}\"" "MESH_FS=\"${meshFS}\"" "SKINNED_MESH_VS=\"${skinnedMeshVS}\"" "SKINNED_MESH_FS=\"${skinnedMeshFS}\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC "MODEL_ALEX=\"${alex}\"" "MODEL_GASMASK=\"${gasmask}\"" "MODEL_KACHUJIN=\"${kachujin}\"" "MODEL_MAW=\"${maw}\"" "MODEL_NINJA=\"${ninja}\"" "MODEL_PALADIN=\"${paladin}\"" "MODEL_VAMPIRE=\"${vampire}\"" "MODEL_WHALE=\"${whale}\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC "MODEL_CUBE=\"${cube}\"" "MODEL_PLANE=\"${plane}\"" "MODEL_SPHERE=\"${sphere}\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC "ANIMATION_CRAWLING=\"${crawling}\"" "ANIMATION_DYING=\"${dying}\"" "ANIMATION_LEG_SWEEP=\"${leg_sweep}\"" "ANIMATION_RUNNING=\"${running}\"" "ANIMATION_SITTING_CLAP=\"${sitting_clap}\"" "ANIMATION_SWING_TO_LAND=\"${swing_to_land}\"" "ANIMATION_TAUNT=\"${taunt}\"" "ANIMATION_WHALE=\"${whale}\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC "DEBUG_TEXTURE=\"${debugTexture}\"")

SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})