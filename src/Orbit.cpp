#include "Orbit.h"
#include <iostream>

Orbit::Orbit(float orbitRadius, float speed, std::vector<GameObject*> children) :
    orbitRadius(orbitRadius), speed(speed), transform({0.0, 0.0, 0.0}),
    time(0.0), children(children) {}

Orbit::~Orbit() {
    for (auto &&child : children) {
        delete child;
    }
}

void Orbit::update(UpdateContext& ctx) {
    for (auto &&child : children) {
        child->update(ctx);
    }
}

void Orbit::physics_update(UpdateContext& ctx) {
    time += ctx.dt;
    float x = orbitRadius * std::cos(time * speed);
    float z = -orbitRadius * std::sin(time * speed);
    transform.position = {x, 0.0, z};

    for (auto &&child : children) {
        child->physics_update(ctx);
    }
}

void Orbit::update_transform(const glm::mat4& parent) {
    transform.computeMatrix();
    transform.computeGlobalMatrix(parent);

    for (auto &&child : children) {
        child->update_transform(transform.globalMatrix);
    }
}

void Orbit::prerender(Shader& shader) {
    for (auto &&child : children) {
        child->prerender(shader);
    }
}

void Orbit::render(Shader& shader) {
    for (auto &&child : children) {
        child->render(shader);
    }
}
