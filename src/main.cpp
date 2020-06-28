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

void drawDragon(Shader_m &dragonShader, glm::vec3 &lightColor, GLTFModel &dragonModel);

void drawPlane(Shader_m &planeShader, glm::vec3 &lightColor, GLTFModel &dragonModel);

void renderQuad();

void setDefferedBuffer(unsigned int &gPosition, unsigned int &gNormal, unsigned int &gAlbedoSpec);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos;
glm::mat4 projection;
glm::mat4 view;

int main() {


    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    ActionListener listener(SCR_WIDTH, SCR_HEIGHT);


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
    GLTFModel dragonModel("../Resource/Model/StandfordDragon/stanford-dragon.gltf");

//    Shader_m dragonShader("../Resource/Shader/CubePractice/materials.vp",
//                          "../Resource/Shader/CubePractice/materials.fp");
//
//    Shader_m boxShader("../Resource/Shader/plane.vp",
//                       "../Resource/Shader/plane.fp");

//    Shader_m dragonShader("../Resource/Shader/DepthShader/materials.vp",
//                          "../Resource/Shader/CubePractice/materials.fp");
//
//    Shader_m boxShader("../Resource/Shader/plane.vp",
//                       "../Resource/Shader/plane.fp");

    Shader_m geometryPass("../Resource/Shader/ssao/geometry.vp", "../Resource/Shader/ssao/geometry.fp");
    Shader_m lightingPass("../Resource/Shader/ssao/light.vp", "../Resource/Shader/ssao/light.fp");

    // configure g-buffer framebuffer
    // ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    unsigned int gPositionDepth, gNormal, gAlbedoSpec;
    setDefferedBuffer(gPositionDepth, gNormal, gAlbedoSpec);

    lightingPass.use();
    lightingPass.setInt("gPositionDepth", 0);
    lightingPass.setInt("gNormal", 1);
    lightingPass.setInt("gAlbedoSpec", 2);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
//        lightPos = glm::vec3(1.2f, 1.0f, -2.0f);
//        lightPos = ActionListener::camera.Position;
        lightPos.x = cos(glfwGetTime()) * 4.0f;
//        lightPos.x = 1.2f;
//        lightPos.y = sin(glfwGetTime()*1.2f);
        lightPos.y = 5.0f;
        lightPos.z = sin(glfwGetTime()) * 4.0f;

        glm::vec3 lightColor = glm::vec3(1.0f);

        projection = glm::perspective(45.0f,
                                      (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      50.0f);
        view = ActionListener::camera.GetViewMatrix();

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

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        geometryPass.use();
        drawDragon(geometryPass, lightColor, dragonModel);
        drawPlane(geometryPass, lightColor, dragonModel);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

//        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(1.0));
//        glm::mat4 lightSpaceMatrix = projection * lightView;
//
//        drawDragon(dragonShader, lightColor, dragonModel);
//
//        drawPlane(boxShader, lightColor, dragonModel);

        // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // -----------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingPass.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPositionDepth);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        // send light relevant uniforms
        // be sure to activate shader when setting uniforms/drawing objects
        lightingPass.setVec3("light.position", lightPos);
        lightingPass.setVec3("viewPos", ActionListener::camera.Position);

        // light properties
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.9f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.9f); // low influence

        lightingPass.setVec3("light.diffuse", diffuseColor);
        lightingPass.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // finally render quad
        renderQuad();

        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        // ----------------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void setDefferedBuffer(unsigned int &gPositionDepth, unsigned int &gNormal, unsigned int &gAlbedoSpec) {
    // position depth color buffer
    glGenTextures(1, &gPositionDepth);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);

    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

//    glGenTextures(1, &gPositionDepth);
//    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
//    // - Normal color buffer
//    glGenTextures(1, &gNormal);
//    glBindTexture(GL_TEXTURE_2D, gNormal);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
//    // - Albedo color buffer
//    glGenTextures(1, &gAlbedoSpec);
//    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void drawDragon(Shader_m &dragonShader,
                glm::vec3 &lightColor, GLTFModel &dragonModel) {

    // material properties
//    0.0	0.1	0.06	0.0	0.50980392	0.50980392	0.50196078	0.50196078	0.50196078	.25
//    glm::vec3 modelColor(0.7686f, 0.8750f, 0.9766f);
    glm::vec3 modelColor(0.5f, 0.7f, 0.9766f);
//    glm::vec3 modelColor(1.0f, 1.0f, 1.0f);
    dragonShader.setVec3("material.diffuse", modelColor);
    dragonShader.setVec3("material.specular", modelColor);
    dragonShader.setFloat("material.shininess", 0.25f * 128);

    // view/projection transformations
//    glm::mat4 projection = glm::perspective(glm::radians(ActionListener::camera.Zoom),
//                                            (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
//                                            100.0f);
//    glm::mat4 view = ActionListener::camera.GetViewMatrix();

    dragonShader.setMat4("projection", projection);
    dragonShader.setMat4("view", view);


    // world transformation
    glm::mat4 dragonModelMat = glm::mat4(1.0f);
    dragonModelMat = glm::translate(dragonModelMat,
                                    glm::vec3(0.0f, -1.0f, 0.0f));
    dragonModelMat = glm::scale(dragonModelMat,
                                glm::vec3(0.2f, 0.2f, 0.2f));
    dragonShader.setMat4("model", dragonModelMat);

    dragonModel.meshes.at(0).Draw(dragonShader);
}

void drawPlane(Shader_m &planeShader,
               glm::vec3 &lightColor, GLTFModel &dragonModel) {

    // material properties
//        pearl	0.25	0.20725	0.20725	1	0.829	0.829	0.296648	0.296648	0.296648	0.088
    planeShader.setVec3("material.diffuse", 1.0f, 0.829f, 0.829f);
    planeShader.setVec3("material.specular", 0.296648f, 0.296648f, 0.296648f);
    planeShader.setFloat("material.shininess", 0.0078125f * 128);

    planeShader.setMat4("projection", projection);
    planeShader.setMat4("view", view);

    glm::mat4 boxModelMat = glm::mat4(1.0f);
    boxModelMat = glm::translate(boxModelMat,
                                 glm::vec3(0.0f, -1.0f, 0.0f));
    boxModelMat = glm::scale(boxModelMat,
                             glm::vec3(1.0f, 1.0f, 1.0f));
    planeShader.setMat4("model", boxModelMat);

    dragonModel.meshes.at(1).Draw(planeShader);
}


void renderQuad()
{
//    cout<<"rend quad"<<endl;
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}