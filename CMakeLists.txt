cmake_minimum_required(VERSION 3.29)
project(QuickCraft)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "source/*.cpp" "source/*.hpp")

add_executable(QuickCraft ${SOURCES})
target_include_directories(QuickCraft PRIVATE source)
