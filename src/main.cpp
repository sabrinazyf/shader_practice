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
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading dragonModel).
    stbi_set_flip_vertically_on_load(true);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile
    // ------------------------------------
    Shader_m dragonShader("../Resource/Shader/CubePractice/materials.vp",
                          "../Resource/Shader/CubePractice/materials.fp");
    GLTFModel dragonModel("../Resource/Model/StandfordDragon/stanford-dragon.gltf");

    Shader_m boxShader("../Resource/Shader/plane.vp",
                       "../Resource/Shader/plane.fp");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        auto currentFrame = (float) glfwGetTime();
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
        dragonShader.use();
        dragonShader.setVec3("light.position", lightPos);
        dragonShader.setVec3("viewPos", ActionListener::camera.Position);

        // light properties
        glm::vec3 lightColor;
//        lightColor.x = sin(glfwGetTime() * 2.0f);
//        lightColor.y = sin(glfwGetTime() * 0.7f);
//        lightColor.z = sin(glfwGetTime() * 1.3f);
        lightColor = glm::vec3(1.0f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence

        dragonShader.setVec3("light.ambient", ambientColor);
        dragonShader.setVec3("light.diffuse", diffuseColor);
        dragonShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        dragonShader.setVec3("material.ambient", 0.7686f, 0.8750f, 0.9766f);
        dragonShader.setVec3("material.diffuse", 0.7686f, 0.8750f, 0.9766f);
        dragonShader.setVec3("material.specular", 0.7686f, 0.8750f, 0.9766f);
        dragonShader.setFloat("material.shininess", 32.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(ActionListener::camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        glm::mat4 view = ActionListener::camera.GetViewMatrix();

        dragonShader.setMat4("projection", projection);
        dragonShader.setMat4("view", view);

        dragonModel.meshes.at(0).Draw(dragonShader);

        // world transformation
        glm::mat4 dragonModelMat = glm::mat4(1.0f);
        dragonModelMat = glm::translate(dragonModelMat,
                                        glm::vec3(0.0f, -1.0f,
                                                  0.0f)); // translate it down so it's at the center of the scene
        dragonModelMat = glm::scale(dragonModelMat,
                                    glm::vec3(0.2f, 0.2f,
                                              0.2f));    // it's a bit too big for our scene, so scale it down
        dragonShader.setMat4("model", dragonModelMat);

        boxShader.use();
        boxShader.setVec3("light.position", lightPos);
        boxShader.setVec3("viewPos", ActionListener::camera.Position);

        boxShader.setVec3("light.ambient", ambientColor);
        boxShader.setVec3("light.diffuse", diffuseColor);
        boxShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        boxShader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
        boxShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
        boxShader.setVec3("material.specular", 1.0f, 1.0f, 1.0f);
        boxShader.setFloat("material.shininess", 5.0f);

        boxShader.setMat4("projection", projection);
        boxShader.setMat4("view", view);

        glm::mat4 boxModelMat = glm::mat4(1.0f);
        boxModelMat = glm::translate(boxModelMat,
                                     glm::vec3(0.0f, -1.0f, 0.0f));
        boxModelMat = glm::scale(boxModelMat,
                                 glm::vec3(1.0f, 1.0f, 1.0f));
        boxShader.setMat4("model", boxModelMat);

        dragonModel.meshes.at(1).Draw(boxShader);

        // render the cube
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
//        vector<Shader_m> dragonShaderVec;
//        dragonShaderVec.push_back(dragonShader);
//        dragonShaderVec.push_back(boxShader);
//        dragonModel.Draw(dragonShaderVec);
//        dragonModel.Draw(dragonShader);
//        boxModel.Draw(boxShader);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

