#include "Texture.h"

#include <stb_image.h>
#include "AppException.h"
#include "GlException.h"

Texture::Texture(const std::string& filename) {
    int width, height, nrChannels;
    stbi_uc *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0); 

    if (data == NULL) {
        throw AppException("failed to read texture file, \"" + filename + "\"");
    }

    GLint format = nrChannels == 4 ? GL_RGBA : GL_RGB;
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);  
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("loading texture", error));
    }

    stbi_image_free(data);
}

Texture::Texture(GLchar r, GLchar g, GLchar b, GLchar a) {
    GLchar data[] = {r,g,b,a};

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("loading vertices", error));
    }
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);
}

void Texture::bind(GLenum texture_unit) {
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}
