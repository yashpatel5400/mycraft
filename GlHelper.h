/*******************************************************************************
 * Filename     :   shader.h
 * Content      :   Shader loading helper code
 * Created      :   June 21, 2020
 * Authors      :   Yash Patel, Joey de Vries (modelled from https://learnopengl.com/Getting-started/Textures)
 * Language     :   C++17
*******************************************************************************/

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glog/logging.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace gl_helper {
    void checkCompileErrors(GLuint shader, std::string type);
    GLuint compile(const char* vShaderCode, const char* fShaderCode);
    void setBool(GLuint program, const std::string& name, bool value);
    void setInt(GLuint program, const std::string& name, int value);
    void setFloat(GLuint program, const std::string& name, float value);
    void setVec2(GLuint program, const std::string& name, const glm::vec2& value);
    void setVec2(GLuint program, const std::string& name, float x, float y);
    void setVec3(GLuint program, const std::string& name, const glm::vec3& value);
    void setVec3(GLuint program, const std::string& name, float x, float y, float z);
    void setVec4(GLuint program, const std::string& name, const glm::vec4& value);
    void setVec4(GLuint program, const std::string& name, float x, float y, float z, float w);
    void setMat2(GLuint program, const std::string& name, const glm::mat2& mat);
    void setMat3(GLuint program, const std::string& name, const glm::mat3& mat);
    void setMat4(GLuint program, const std::string& name, const glm::mat4& mat);
}