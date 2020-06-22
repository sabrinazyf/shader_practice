#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

using namespace std;

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

void size_callback(GLFWwindow *window, int width, int height) {
    //printf("width: %d\theight: %d\n", width, height);
    glViewport(0, 0, width, height);
}

int main() {

    GLFWwindow *window;
    glfwInit();
    window = glfwCreateWindow(1024, 600, "Demo", NULL, NULL);
    glfwSetWindowSizeCallback(window, size_callback);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

//    printf("OpenGL %s, GLSL %s", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    cout<<"OpenGL "<<glGetString(GL_VERSION)<<" glGetString(GL_SHADING_LANGUAGE_VERSION)"<<endl;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_LINE_STRIP);
        glVertex2f(0.1, 0.1);
        glVertex2f(0.3, 0.5);
        glVertex2f(0.4, -0.1);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}