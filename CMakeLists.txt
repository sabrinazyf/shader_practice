cmake_minimum_required(VERSION 3.14)
project(ShaderPractice)

set (CMAKE_CXX_STANDARD 14)
#set (CMAKE_USE_RELATIVE_PATHS ON)
include_directories(include)
link_directories(lib)

#MESSAGE(STATUS ${LIBHELLO_PATH} " found")

add_executable(ShaderPractice src/main.cpp src/glad.c)
target_link_libraries(ShaderPractice glew32s glfw3 gdi32 opengl32)