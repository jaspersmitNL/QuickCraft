cmake_minimum_required(VERSION 3.22)
project(QuickCraft)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
        GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

# load Dawn.cmake
include(Dawn.cmake)


file(GLOB_RECURSE SOURCES source/*.cpp source/include/*.hpp)


add_executable(QuickCraft ${SOURCES})
target_include_directories(QuickCraft PRIVATE source/include)
target_link_libraries(QuickCraft PRIVATE dawn::webgpu_dawn glfw webgpu_glfw glm::glm)
target_copy_webgpu_binaries(QuickCraft)
