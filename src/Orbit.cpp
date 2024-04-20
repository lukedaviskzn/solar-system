#include "Orbit.h"
#include <iostream>

Orbit::Orbit(double orbitRadius, double speed, std::vector<GameObject*> children) :
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
    double x = orbitRadius * std::cos(time * speed);
    double z = -orbitRadius * std::sin(time * speed);
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

void Orbit::prerender(Shader& shader, UpdateContext& ctx) {
    for (auto &&child : children) {
        child->prerender(shader, ctx);
    }
}

void Orbit::render(Shader& shader, UpdateContext& ctx) {
    for (auto &&child : children) {
        child->render(shader, ctx);
    }
}
