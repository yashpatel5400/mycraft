/*******************************************************************************
 * Filename     :   Character.h
 * Content      :   Character class (interacts with the scene and has time update)
 * Created      :   June 21, 2020
 * Authors      :   Yash Patel
 * Language     :   C++17
*******************************************************************************/

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Scene.h"

struct Character {
    Character() = default;
    Character(const glm::vec2& position, const glm::vec2& velocity);
    void tick(const Scene& scene, float delta);
    void draw(const GLuint program);

    GLuint m_VAO;
    GLuint m_VBO;
    glm::vec2 m_position; // corresponds to the bottom left point (assumes character square)
    glm::vec2 m_velocity;
};