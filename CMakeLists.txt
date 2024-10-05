cmake_minimum_required(VERSION 3.29)
project(QuickCraft)

set(CMAKE_CXX_STANDARD 23)

include(vendor/Dawn.cmake)
add_subdirectory(vendor/glm)

file(GLOB_RECURSE SOURCES "source/*.cpp" "source/*.hpp")
file(GLOB_RECURSE IMGUI_SOURCES "vendor/imgui/*.cpp" "vendor/imgui/*.h")

add_executable(QuickCraft ${SOURCES} ${IMGUI_SOURCES})
target_include_directories(QuickCraft PRIVATE source vendor/imgui)
target_link_libraries(QuickCraft PRIVATE dawn::webgpu_dawn glfw webgpu_glfw glm)
target_copy_webgpu_binaries(QuickCraft)
