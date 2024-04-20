#pragma once

#include <fstream>
#include <GL/glew.h>

class Texture
{
private:
    GLuint texture;
public:
    Texture(const std::string& filename);
    Texture(GLchar r, GLchar g, GLchar b, GLchar a);
    Texture(const Texture& _) = delete;
    void operator=(const Texture& _) = delete;
    ~Texture();
    
    void bind(GLenum texture_unit);
};
