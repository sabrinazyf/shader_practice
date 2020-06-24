cmake_minimum_required(VERSION 3.14)
project(ShaderPractice)

set(CMAKE_CXX_STANDARD 14)
#set (CMAKE_USE_RELATIVE_PATHS ON)
include_directories(include)
link_directories(lib)

#MESSAGE(STATUS ${LIBHELLO_PATH} " found")

add_executable(ShaderPractice src/main.cpp src/glad.c
        include/header/Mesh.cpp include/header/Mesh.h
        include/header/Model.cpp include/header/Model.h
        include/header/Shader.cpp include/header/Shader.h
        include/header/Camera.cpp include/header/Camera.h
        )
target_link_libraries(ShaderPractice glew32 glfw3 gdi32 opengl32 assimp)