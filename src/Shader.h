#pragma once

#include <string>
#include <fstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>

class Shader
{
private:
    GLuint program;

    void init(const char* vertSrc, const char* fragSrc);
public:
    Shader(const std::string& vertSource, const std::string& fragSource);
    Shader(std::ifstream& vertFile, std::ifstream& fragFile);
    Shader(const Shader& _) = delete;
    void operator=(const Shader& _) = delete;
    ~Shader();
    
    void use();
    void setTextureUnit(const std::string& name, GLint unit_number);
    void setUniform(const std::string& name, int uniform);
    void setUniform(const std::string& name, float uniform);
    void setUniform(const std::string& name, const glm::mat4& uniform);
};
