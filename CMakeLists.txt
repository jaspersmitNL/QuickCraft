cmake_minimum_required(VERSION 3.22)
project(QuickCraft)

set(CMAKE_CXX_STANDARD 23)

# load Dawn.cmake
include(Dawn.cmake)


file(GLOB_RECURSE SOURCES source/*.cpp source/include/*.hpp)


add_executable(QuickCraft ${SOURCES})
target_include_directories(QuickCraft PRIVATE source/include)
target_link_libraries(QuickCraft PRIVATE dawn::webgpu_dawn glfw webgpu_glfw)
target_copy_webgpu_binaries(QuickCraft)
