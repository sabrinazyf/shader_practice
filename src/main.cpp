#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <header/Shader_m.h>
#include <header/Camera.h>
#include <header/GLTFModel.h>

#include <iostream>
#include "tools/ActionListener.h"


using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

//
//void mouse_move_callback(GLFWwindow *window, double xpos, double ypos);
//
//void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
//

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    ActionListener listener(SCR_WIDTH, SCR_HEIGHT);

    lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, ActionListener::mouse_move_callback);
    glfwSetScrollCallback(window, ActionListener::scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    Shader_m ourShader("../Resource/Shader/CubePractice/materials.vp",
                             "../Resource/Shader/CubePractice/materials.fp");
    GLTFModel model("../Resource/Model/StandfordDragon/stanford-dragon.gltf",
                    "../Resource/Model/StandfordDragon/stanford-dragon.bin");
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        auto currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        ActionListener::setDeltaTime(deltaTime);
        lastFrame = currentFrame;

        // input
        // -----
        ActionListener::processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        ourShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(ActionListener::camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = ActionListener::camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 modelMat = glm::mat4(1.0f);
        // translate it down so it's at the center of the scene
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f));
        // it's a bit too big for our scene, so scale it down
        modelMat = glm::scale(modelMat, glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setMat4("model", modelMat);

//        glm::vec3 lightColor;
//        lightColor.x = sin(glfwGetTime() * 2.0f);
//        lightColor.y = sin(glfwGetTime() * 0.7f);
//        lightColor.z = sin(glfwGetTime() * 1.3f);
//        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
//        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
//        ourShader.setVec3("light.ambient", ambientColor);
//        ourShader.setVec3("light.diffuse", diffuseColor);
//        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
//
//        // material properties
//        ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
//        ourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
//        ourShader.setVec3("material.specular", 0.5f, 0.5f,
//                               0.5f); // specular lighting doesn't have full effect on this object's material
//        ourShader.setFloat("material.shininess", 32.0f);

        model.Draw(ourShader);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
//    glDeleteVertexArrays(1, &cubeVAO);
//    glDeleteVertexArrays(1, &lightCubeVAO);
//    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

