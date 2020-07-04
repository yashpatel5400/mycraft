/*******************************************************************************
 * Filename     :   main.cpp
 * Content      :   Main game loop
 * Created      :   June 21, 2020
 * Authors      :   Yash Patel
 * Language     :   C++17
*******************************************************************************/

#include <iostream>
#include <mutex>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "Character.h"
#include "GlHelper.h"
#include "Scene.h"

const char* vertexShader = R"(
    #version 420 core

    in vec2 a_pos;

    void main() {
        gl_Position = vec4(a_pos, 0.0, 1.0);
    }
)";

const char* fragmentShader = R"(
    #version 420 core

    uniform vec3 u_color;

    out vec4 FragColor;

    void main() {
        FragColor = vec4(u_color, 1.0);
    }
)";

const unsigned int SCR_SIZE = 1000;

float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

Character character;
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } 
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        character.m_velocity += glm::vec2(0.0, 1.0);
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        character.m_velocity -= glm::vec2(1.0, 0.0);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        character.m_velocity -= glm::vec2(0.0, 1.0);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        character.m_velocity += glm::vec2(1.0, 0.0);
    }
}

int main(int argc, char** argv) {
    FLAGS_stderrthreshold = 0;
    FLAGS_logtostderr = 0;

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_SIZE, SCR_SIZE, "SDF", NULL, NULL);
    CHECK(window != NULL) << "Failed to create GLFW window";

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    CHECK(GLEW_OK == err) << "Failed to initialize GLEW";
    GLuint program = gl_helper::compile(vertexShader, fragmentShader);

    Platform ground(glm::vec2(-1.0, -1.0), glm::vec2(1.0, 0.0));
    std::vector platforms = { ground };
    Scene scene(platforms);

    float previousTime = glfwGetTime();
    character = Character(glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0));
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float kTimeStep = 0.50; // without this, we may take too large steps and clip into objects
        float currentTime = glfwGetTime();
        float delta = kTimeStep * (currentTime - previousTime);
        character.tick(scene, delta);
        previousTime = currentTime;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        character.draw(program);
        scene.draw(program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}