#ifndef SHADERPRACTICE_ACTIONLISTENER_H
#define SHADERPRACTICE_ACTIONLISTENER_H

#include <header/Camera.h>
#include <GLFW/glfw3.h>

class ActionListener {
public:
    static Camera camera;

    ActionListener(int SCR_WIDTH, int SCR_HEIGHT);

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
    static void mouse_move_callback(GLFWwindow *window, double xpos, double ypos);


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
    static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
    static void processInput(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    static void setDeltaTime(float d){
        deltaTime = d;
    }

private:
    static float lastX;
    static float lastY;
    static bool firstMouse;

// timing
    static float deltaTime;
//    float lastFrame;
};


#endif //SHADERPRACTICE_ACTIONLISTENER_H
