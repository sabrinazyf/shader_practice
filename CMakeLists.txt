cmake_minimum_required(VERSION 3.14)
project(ShaderPractice)

set(CMAKE_CXX_STANDARD 14)
#set (CMAKE_USE_RELATIVE_PATHS ON)
include_directories(include)
link_directories(lib)

#MESSAGE(STATUS ${LIBHELLO_PATH} " found")

add_executable(ShaderPractice src/main.cpp src/glad.c
#        src/header/Mesh.cpp src/header/Mesh.h
#        src/header/Model.cpp src/header/Model.h
#        src/header/Shader.cpp src/header/Shader.h
#        src/header/Camera.cpp src/header/Camera.h
        )
target_link_libraries(ShaderPractice glew32 SOIL glfw3 gdi32 opengl32)