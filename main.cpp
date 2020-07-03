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

    uniform vec3 u_color;

    out vec4 FragColor;

    void main() {
        FragColor = vec4(u_color, 1.0);
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
        // physical constants
        constexpr float kG = 9.8; // gravitational constant m/s2
        constexpr float kMu = 10.0; // assumed universal kinect friction unitless

        // need to do the update atomically, so stored in temporary and update at end
        glm::vec2 gravity(0.0, -kG);
        glm::vec2 friction = m_velocity.x * glm::vec2(-kMu, 0.0);

        glm::vec2 velocity = m_velocity + (gravity + friction) * delta;
        glm::vec2 position = m_position + m_velocity * delta;

        if (m_position.y <= 0.0) {
            velocity.y = 0.0; // collides with the ground
        }

        m_velocity = velocity;
        m_position = position;
    }

    glm::vec2 m_position;
    glm::vec2 m_velocity;
};

Character character(glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0));

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

    constexpr float kSquareSize = 0.25;
    float charVertices[] = {
        -kSquareSize, 0.0,
        kSquareSize, 0.0,
        -kSquareSize, 2 * kSquareSize,
        kSquareSize, 2 * kSquareSize
    };

    unsigned int VBO, charVAO;
    glGenVertexArrays(1, &charVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(charVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(charVertices), charVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float groundVertices[] = {
        -1.0, -1.0,
        -1.0, 0.0,
        1.0, -1.0,
        1.0, 0.0,
    };

    unsigned int groundVBO, groundVAO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);

    glBindVertexArray(groundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float previousTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float kTimeStep = 0.50; // without this, we may take too large steps and clip into objects
        float currentTime = glfwGetTime();
        float delta = kTimeStep * (currentTime - previousTime);
        character.tick(delta);
        previousTime = currentTime;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        // draw the character
        gl_helper::setVec2(program, "u_charPos", character.m_position);
        gl_helper::setVec3(program, "u_color", glm::vec3(1.0, 0.0, 0.0));
        glBindVertexArray(charVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // draw the ground
        gl_helper::setVec2(program, "u_charPos", glm::vec2(0.0));
        gl_helper::setVec3(program, "u_color", glm::vec3(0.0, 1.0, 0.0));
        glBindVertexArray(groundVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &charVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}