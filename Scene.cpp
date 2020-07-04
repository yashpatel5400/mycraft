/*******************************************************************************
 * Filename     :   Character.cpp
 * Content      :   Character class (interacts with the scene and has time update)
 * Created      :   June 21, 2020
 * Authors      :   Yash Patel
 * Language     :   C++17
*******************************************************************************/

#include "Scene.h"

#include <glog/logging.h>

#include "GlHelper.h"

// collidable platform, assumed axis aligned for now
Platform::Platform(const glm::vec2& bottomLeft, const glm::vec2& topRight) :
    m_topRight(topRight),
    m_bottomLeft(bottomLeft) {

    float vertices[] = {
        m_bottomLeft.x, m_bottomLeft.y,
        m_topRight.x, m_bottomLeft.y,
        m_bottomLeft.x, m_topRight.y,
        m_topRight.x, m_topRight.y,
    };

    unsigned int VBO;
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

bool Platform::collides(const glm::vec2& position) const {
    return position.y <= m_topRight.y;
}

void Platform::draw(const GLuint program) const {
    glUseProgram(program);
    gl_helper::setVec3(program, "u_color", glm::vec3(0.0, 1.0, 0.0));
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

Scene::Scene(const std::vector<Platform>& platforms) :
    m_platforms(platforms) {};

bool Scene::collides(const glm::vec2& position) const {
    for (const auto& platform : m_platforms) {
        if (platform.collides(position)) {
            return true;
        }
    }
    return false;
}

void Scene::draw(const GLuint program) const {
    for (const auto& platform : m_platforms) {
        platform.draw(program);
    }
}