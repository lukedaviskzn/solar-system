#include "Mesh.h"

#include <iostream>
#include <sstream>
#include "GlException.h"
#include "AppException.h"

void Mesh::loadVertices(const std::vector<Vertex>& vertices) {
    vbo_length = vertices.size();

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)8);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)20);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("loading vertices", error));
    }
}

Mesh::Mesh(const std::vector<Vertex>& vertices): indexed(false) {
    loadVertices(vertices);
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, bool flip_order): indexed(true) {
    loadVertices(vertices);

    ebo_length = indices.size();

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    if (!flip_order) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    } else {
        std::vector<GLuint> new_indices;
        new_indices.reserve(ebo_length);

        for (size_t i = 0; i < ebo_length / 3; i++) {
            new_indices.push_back(indices[3*i + 2]);
            new_indices.push_back(indices[3*i + 1]);
            new_indices.push_back(indices[3*i + 0]);
        }
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &new_indices[0], GL_STATIC_DRAW);
    }

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("loading indices", error));
    }
}

std::unique_ptr<Mesh> Mesh::LoadObj(std::ifstream& file, bool flip_order) {
    std::string line;

    std::vector<Vertex> vertices;
    std::vector<glm::vec2> tex_coords;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;

    while (std::getline(file, line)) {
        if (line.size() == 0 || line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        else if (line.rfind("v ", 0) == 0) {
            std::stringstream ss(line.substr(2));
            std::string token;
            
            std::getline(ss, token, ' ');
            GLfloat x = std::stof(token);

            std::getline(ss, token, ' ');
            GLfloat y = std::stof(token);

            std::getline(ss, token, ' ');
            GLfloat z = std::stof(token);

            vertices.push_back({glm::vec3(x, y, z), glm::vec2(0.0), glm::vec3(0.0)});
        } else if (line.rfind("vt ", 0) == 0) {
            std::stringstream ss(line.substr(3));
            std::string token;
            
            std::getline(ss, token, ' ');
            GLfloat u = std::stof(token);
            
            std::getline(ss, token, ' ');
            GLfloat v = std::stof(token);

            tex_coords.push_back({u, v});
        } else if (line.rfind("vn ", 0) == 0) {
            std::stringstream ss(line.substr(3));
            std::string token;
            
            std::getline(ss, token, ' ');
            GLfloat x = std::stof(token);
            
            std::getline(ss, token, ' ');
            GLfloat y = std::stof(token);
            
            std::getline(ss, token, ' ');
            GLfloat z = std::stof(token);

            normals.push_back({x, y, z});
        } else if (line.rfind("f ", 0) == 0) {
            std::stringstream ss(line.substr(2));

            GLuint fIdx[4];

            std::string face;

            int i = 0;

            while (std::getline(ss, face, ' ')) {
                std::string token;
                std::stringstream fss(face);

                std::getline(fss, token, '/');
                GLuint vIndex = std::stoul(token);
                
                std::getline(fss, token, '/');
                GLuint tIndex = std::stoul(token);
                
                std::getline(fss, token, '/');
                GLuint nIndex = std::stoul(token);

                vertices[vIndex-1].tex_coords = tex_coords[tIndex-1];
                vertices[vIndex-1].normal = normals[tIndex-1];

                fIdx[i] = vIndex-1;

                i++;
            }
            
            if (i > 3) {
                // stitch into 2 triangles
                indices.push_back(fIdx[0]);
                indices.push_back(fIdx[1]);
                indices.push_back(fIdx[3]);
                indices.push_back(fIdx[1]);
                indices.push_back(fIdx[2]);
                indices.push_back(fIdx[3]);
            } else {
                indices.push_back(fIdx[0]);
                indices.push_back(fIdx[1]);
                indices.push_back(fIdx[2]);
            }
        } else {
            // unrecognised character
            // throw(AppException("failed to load obj file, unrecognised character '" + line[0] + std::string("' in line '") + line + "'."));
            // break;
        }
    }

    if (flip_order) {
        for (size_t i = 0; i < indices.size() / 3; i++) {
            GLuint temp = indices[i*3 + 0];
            indices[i*3 + 0] = indices[i*3 + 2];
            indices[i*3 + 2] = temp;
        }
    }

    if (indices.size() > 0) {
        return std::make_unique<Mesh>(vertices, indices);
    } else {
        return std::make_unique<Mesh>(vertices);
    }
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::draw(bool write_depth) {
    glBindVertexArray(vao);

    glDepthMask(write_depth);
    
    if (!indexed) {
        glDrawArrays(GL_TRIANGLES, 0, vbo_length);
    } else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, ebo_length, GL_UNSIGNED_INT, 0);
    }

    glDepthMask(GL_TRUE);

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        throw(GlException("drawing mesh", error));
    }
}
