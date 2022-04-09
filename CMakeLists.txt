cmake_minimum_required(VERSION 3.18)
project(SkeletalAnimation)

set(CMAKE_CXX_STANDARD 20)

# GLOB_RECURSE for recursive search
FILE(GLOB_RECURSE cppSources src/*.cpp)
FILE(GLOB_RECURSE hppSources src/*.hpp)
set(sources
    ${cppSources}
    ${hppSources}
)

# shaders
find_file(meshVS meshVS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(meshFS meshFS.glsl
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

# animations
find_file(legSweep leg_sweep.gltf
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res/animations
)

add_executable(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/3rd/include)
target_link_libraries(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/3rd/lib/SDL2/SDL2maind.lib
    ${CMAKE_CURRENT_LIST_DIR}/3rd/lib/SDL2/SDL2d.lib
    ${CMAKE_CURRENT_LIST_DIR}/3rd/lib/GLEW/glew32s.lib
    #${CMAKE_CURRENT_LIST_DIR}/3rd/lib/SDL2/SDL2-staticd.lib
    opengl32.lib
)
target_compile_definitions(${PROJECT_NAME} PUBLIC "MESH_VS=\"${meshVS}\"" "MESH_FS=\"${meshFS}\"" "MODEL_ALEX=\"${alex}\"" "MODEL_GASMASK=\"${gasmask}\"" "MODEL_KACHUJIN=\"${kachujin}\"" "MODEL_MAW=\"${maw}\"" "MODEL_NINJA=\"${ninja}\"" "MODEL_PALADIN=\"${paladin}\"" "MODEL_VAMPIRE=\"${vampire}\"" "ANIMATION_LEG_SWEEP=\"${legSweep}\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC "DEBUG_TEXTURE=\"${debugTexture}\"")

SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})