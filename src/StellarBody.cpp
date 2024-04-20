#include "StellarBody.h"

#include <iostream>

StellarBody::StellarBody(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> texture_night, std::shared_ptr<Mesh> mesh, float scale, float rotSpeed, float rotAngle)
    : texture(texture), texture_night(texture_night), mesh(mesh), has_night(true), lighting(1), rotSpeed(rotSpeed) {
    glm::quat q = glm::angleAxis(rotAngle, glm::vec3(0.0f, 0.0f, -1.0f));
    rotAxis = q * glm::vec3(0.0f, 1.0f, 0.0f);
    transform.rotation *= q;
    transform.scale *= scale;
}

StellarBody::StellarBody(std::shared_ptr<Texture> texture, int lighting, std::shared_ptr<Mesh> mesh, float scale, float rotSpeed, float rotAngle)
    : texture(texture), texture_night(texture), mesh(mesh), has_night(false), lighting(lighting), rotSpeed(rotSpeed) {
    glm::quat q = glm::angleAxis(rotAngle, glm::vec3(0.0f, 0.0f, -1.0f));
    rotAxis = q * glm::vec3(0.0f, 1.0f, 0.0f);
    transform.rotation *= q;
    transform.scale *= scale;
}

void StellarBody::physics_update(UpdateContext& ctx) {
    // transform.rotation *= glm::angleAxis(ctx.dt, rotAxis);
    transform.rotation *= glm::angleAxis(ctx.dt * rotSpeed, glm::vec3(0.0, 1.0, 0.0));
}

void StellarBody::update_transform(const glm::mat4& parent) {

    transform.computeMatrix();
    transform.computeGlobalMatrix(parent);
}

void StellarBody::render(Shader& shader) {
    shader.setUniform("model", transform.globalMatrix);
    shader.setUniform("has_night", has_night);
    shader.setUniform("lighting", lighting);

    texture->bind(GL_TEXTURE0);
    if (has_night) {
        texture_night->bind(GL_TEXTURE1);
    }

    shader.setTextureUnit("albedo", 0);
    shader.setTextureUnit("albedo_night", 1);

    mesh->draw();
}
