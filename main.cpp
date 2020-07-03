/*******************************************************************************
 * Filename     :   main.cpp
 * Content      :   Main game loop
 * Created      :   June 21, 2020
 * Authors      :   Yash Patel
 * Language     :   C++17
*******************************************************************************/

#include <iostream>
#include <filesystem>
#include <mutex>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shader.h"

const char* vertexShader = R"(
    #version 420 core

    uniform vec2 u_charPos;

    in vec2 a_pos;

    void main() {
        gl_Position = vec4(a_pos + u_charPos, 0.0, 1.0);
    }
)";

const char* fragmentShader = R"(
    #version 420 core

    out vec4 FragColor;

    void main() {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
)";

const unsigned int SCR_SIZE = 1000;

float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

struct Character {
    Character(const glm::vec2& position,
              const glm::vec2& velocity) :
        m_position(position),
        m_velocity(velocity) {

    }

    void tick(float delta) {
        m_position += m_velocity * delta;
    }

    glm::vec2 m_position;
    glm::vec2 m_velocity;
};

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

    Character character(glm::vec2(0.0, 0.0), glm::vec2(0.0, 1.0));

    constexpr float kSquareSize = 0.25;
    float vertices[] = {
        -kSquareSize, -kSquareSize,
        kSquareSize, -kSquareSize,
        -kSquareSize, kSquareSize,
        kSquareSize, kSquareSize
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float previousTime = glfwGetTime();;
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float delta = currentTime - previousTime;
        character.tick(delta);
        previousTime = currentTime;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        gl_helper::setVec2(program, "u_charPos", character.m_position);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
