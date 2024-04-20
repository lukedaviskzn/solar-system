#include "Shader.h"

#include <sstream>
#include <GL/glu.h>
#include "AppException.h"
#include "GlException.h"
#include "Mesh.h"

void Shader::init(const char* vertSrc, const char* fragSrc) {
    // Compile vertex shader
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertSrc, NULL);
    glCompileShader(vert);

    // Compile fragment shader
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragSrc, NULL);
    glCompileShader(frag);
    
    // Link shaders to program
    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        char log[2048];
        glGetProgramInfoLog(program, 2048, NULL, log);
        throw(AppException(log));
    }

    // Shaders no longer needed
    glDeleteShader(vert);
    glDeleteShader(frag);

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("creating shader", error));
    }
}

Shader::Shader(const std::string& vertSource, const std::string& fragSource) {
    init(vertSource.c_str(), fragSource.c_str());
}

Shader::Shader(std::ifstream& vertFile, std::ifstream& fragFile) {
    std::stringstream vertBuffer;
    vertBuffer << vertFile.rdbuf();

    std::stringstream fragBuffer;
    fragBuffer << fragFile.rdbuf();

    init(vertBuffer.str().c_str(), fragBuffer.str().c_str());
}

Shader::~Shader() {
    glDeleteProgram(program);
}

void Shader::use() {
    if (!glIsProgram(program)) {
        throw(AppException("program freed early"));
    }

    glUseProgram(program);

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("using shader", error));
    }
}

void Shader::setTextureUnit(const std::string& name, GLint unit_number) {
    use();
    
    GLint loc = glGetUniformLocation(program, name.c_str());

    glUniform1i(loc, unit_number);

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("set shader uniform (texture unit)", error));
    }
}

void Shader::setUniform(const std::string& name, int uniform) {
    use();

    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform1i(loc, uniform);

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("set shader uniform (int)", error));
    }
}

void Shader::setUniform(const std::string& name, float uniform) {
    use();

    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform1f(loc, uniform);

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("set shader uniform (float)", error));
    }
}

void Shader::setUniform(const std::string& name, const glm::mat4& uniform) {
    use();

    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, &uniform[0][0]);

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("set shader uniform (mat4)", error));
    }
}
