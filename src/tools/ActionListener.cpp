
#include "ActionListener.h"
float ActionListener::lastX;
float ActionListener::lastY;
bool ActionListener::firstMouse;
Camera ActionListener::camera;
float ActionListener::deltaTime;

// timing
float deltaTime;
ActionListener::ActionListener(int SCR_WIDTH, int SCR_HEIGHT) {
    ActionListener::camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    ActionListener::lastX = (float)SCR_WIDTH / 2.0f;
    ActionListener::lastY = (float)SCR_HEIGHT / 2.0f;
    ActionListener::firstMouse = true;

// timing
    ActionListener::deltaTime = 0.0f;
//    this->lastFrame = 0.0f;
}

void ActionListener::mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE
        && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        firstMouse = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (firstMouse) {
            lastX = (float)xpos;
            lastY = (float)ypos;
            firstMouse = false;
        }

        float xoffset = (float)xpos - lastX;
        float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

        lastX = (float)xpos;
        lastY = (float)ypos;

        camera.ProcessRightMouseMovement(xoffset, yoffset);
    }

//    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
//        if (!firstMouse)
//            firstMouse = true;
//    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (firstMouse) {
            lastX = (float)xpos;
            lastY = (float)ypos;
            firstMouse = false;
//            cout << "first mouse false" << endl;
        }
//        cout << "This Pos:" << xpos << ", " << ypos << endl;
//        cout << "Last Pos:" << lastX << ", " << lastY << endl;

        float xoffset = (float)xpos - lastX;
        float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top
//        cout << "Offset:" << xoffset << ", " << yoffset << endl << endl;

        lastX = (float)xpos;
        lastY = (float)ypos;

        camera.ProcessLeftMouseMovement(xoffset, yoffset, deltaTime);
    }
}

void ActionListener::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
//    std::cout<<yoffset<<std::endl;
    camera.ProcessMouseScroll((float)yoffset);
}
