/*******************************************************************************
 * Filename     :   Character.cpp
 * Content      :   Character class (interacts with the scene and has time update)
 * Created      :   June 21, 2020
 * Authors      :   Yash Patel
 * Language     :   C++17
*******************************************************************************/

#include "Character.h"

#include <glog/logging.h>

#include "GlHelper.h"

Character::Character(const glm::vec2& position,
    const glm::vec2& velocity) :
    m_position(position),
    m_velocity(velocity) {

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
}

void Character::tick(const Scene& scene, float delta) {
    // physical constants
    constexpr float kG = 9.8; // gravitational constant m/s2
    constexpr float kMu = 10.0; // assumed universal kinect friction unitless

    // need to do the update atomically, so stored in temporary and update at end
    glm::vec2 gravity(0.0, -kG);
    glm::vec2 friction = m_velocity.x * glm::vec2(-kMu, 0.0);

    glm::vec2 velocity = m_velocity + (gravity + friction) * delta;
    glm::vec2 position = m_position + m_velocity * delta;

    if (scene.collides(m_position)) {
        velocity.y = 0.0; // collides with the ground
    }

    m_velocity = velocity;
    m_position = position;
}

void Character::draw(const GLuint program) {
    constexpr float kSquareSize = 0.25;
    float vertices[] = {
        m_position.x, m_position.y,
        m_position.x + kSquareSize, m_position.y,
        m_position.x, m_position.y + kSquareSize,
        m_position.x + kSquareSize, m_position.y + kSquareSize
    };

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(program);
    gl_helper::setVec3(program, "u_color", glm::vec3(1.0, 0.0, 0.0));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}