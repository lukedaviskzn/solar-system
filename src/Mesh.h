#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <memory>

struct Vertex {
    glm::vec3 position;
    glm::vec2 tex_coords;
    glm::vec3 normal;
};

class Mesh
{
private:
    GLuint vbo;
    GLuint vao;
    GLuint vbo_length;
    GLuint ebo;
    GLuint ebo_length;
    bool indexed = false;

    void loadVertices(const std::vector<Vertex>& vertices);
public:
    Mesh(const std::vector<Vertex>& vertices);
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, bool flip_order = false);
    Mesh(const Mesh& _) = delete;
    void operator=(const Mesh& _) = delete;
    ~Mesh();
    static std::unique_ptr<Mesh> LoadObj(std::ifstream& file, bool flip_order = false);

    void draw(bool write_depth = true);
};
