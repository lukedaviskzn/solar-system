#include "Sky.h"

#include <iostream>

Sky::Sky(std::shared_ptr<Texture> texture, std::shared_ptr<Mesh> mesh, float scale)
    : texture(texture), mesh(mesh), scale(scale) {}

void Sky::render(Shader& shader, UpdateContext& ctx) {
    glm::mat4 matrix(1.0);
    matrix = glm::translate(matrix, glm::vec3(ctx.camera_pos));

    shader.setUniform("model", matrix);
    shader.setUniform("has_night", false);
    shader.setUniform("lighting", 0);

    texture->bind(GL_TEXTURE0);

    shader.setTextureUnit("albedo", 0);
    shader.setTextureUnit("albedo_night", 1);

    mesh->draw(false);
}
