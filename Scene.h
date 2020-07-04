/*******************************************************************************
 * Filename     :   Character.h
 * Content      :   Character class (interacts with the scene and has time update)
 * Created      :   June 21, 2020
 * Authors      :   Yash Patel
 * Language     :   C++17
*******************************************************************************/

#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

// collidable platform, assumed axis aligned for now
struct Platform {
    Platform(const glm::vec2& bottomLeft, const glm::vec2& topRight);
    bool collides(const glm::vec2& position) const;
    void draw(const GLuint program) const;

    GLuint m_VAO;
    glm::vec2 m_topRight;
    glm::vec2 m_bottomLeft;
};

struct Scene {
    Scene(const std::vector<Platform>& platforms);
    bool collides(const glm::vec2& position) const;
    void draw(const GLuint program) const;

    std::vector<Platform> m_platforms;
};