cmake_minimum_required(VERSION 3.18)

set(CMAKE_CONFIGURATION_TYPES Debug Release Distribution)
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

project(SkeletalAnimation)

# BUILD TYPES
# Default flags
message("CMAKE_CXX_FLAGS:" ${CMAKE_CXX_FLAGS})
message("CMAKE_CXX_FLAGS_DEBUG:" ${CMAKE_CXX_FLAGS_DEBUG})
message("CMAKE_CXX_FLAGS_RELEASE:" ${CMAKE_CXX_FLAGS_RELEASE})
message("CMAKE_CXX_FLAGS_RELWITHDEBINFO:" ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
message("CMAKE_CXX_FLAGS_MINSIZEREL:" ${CMAKE_CXX_FLAGS_MINSIZEREL})

# "Distribution" build (for installation)
set(CMAKE_CXX_FLAGS_DISTRIBUTION "${CMAKE_CXX_FLAGS_RELEASE}")
message("CMAKE_CXX_FLAGS_DISTRIBUTION:" ${CMAKE_CXX_FLAGS_DISTRIBUTION})

# SOURCES
FILE(GLOB_RECURSE cppSources src/*.cpp)
FILE(GLOB_RECURSE hppSources src/*.hpp)
set(sources
    ${cppSources}
    ${hppSources}
)
# Preserve file system folder structure in Visual Studio
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src PREFIX "Source Files" FILES ${cppSources})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src PREFIX "Header Files" FILES ${hppSources})

add_executable(${PROJECT_NAME} ${sources})

# EXTERNAL LIBRARIES
add_subdirectory(vendor/glew)
add_subdirectory(vendor/SDL2)
add_library(imgui
    # Main Imgui files
    vendor/imgui/imgui.cpp vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp vendor/imgui/imgui_widgets.cpp
    # SDL2+OpenGL-specific files
    vendor/imgui/backends/imgui_impl_sdl.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/SDL2/include)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/glew/include/GL)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/ ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/backends/)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<glew.h>  GL_GLEXT_PROTOTYPES=1)

# INCLUDES
target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/glew/include/GL
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/glm/include
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/SDL2/include
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/vendor/tinyGLTF/include
)

# LINKER
target_link_libraries(${PROJECT_NAME}
    libglew_static
    SDL2main
    SDL2-static
    opengl32.lib
    imgui
)

# MACROS
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# INSTALLATION
FILE(GLOB animFiles res/animations/*)
FILE(GLOB imageFiles res/images/*)
FILE(GLOB modelFiles res/models/*)
FILE(GLOB shaderFiles shaders/*)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES ${animFiles} DESTINATION res/animations)
install(FILES ${imageFiles} DESTINATION res/images)
install(FILES ${modelFiles} DESTINATION res/models)
install(FILES ${shaderFiles} DESTINATION shaders/)

# PACKAGING (INSTALLER GENERATION)
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/installers)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
include(CPack)