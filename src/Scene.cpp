#include "Scene.h"

#include <iostream>

Scene::Scene(std::vector<GameObject*> children): children(children) {}

Scene::~Scene() {
    for (auto &&child : children) {
        delete child;
    }
}

void Scene::update(UpdateContext& ctx) {
    for (auto &&child : children) {
        child->update(ctx);
    }
}

void Scene::physics_update(UpdateContext& ctx) {
    for (auto &&child : children) {
        child->physics_update(ctx);
    }
}

void Scene::update_transform(const glm::mat4& parent) {
    for (auto &&child : children) {
        child->update_transform(parent);
    }
}

void Scene::prerender(Shader& shader, UpdateContext& ctx) {
    for (auto &&child : children) {
        child->prerender(shader, ctx);
    }
}

void Scene::render(Shader& shader, UpdateContext& ctx) {
    for (auto &&child : children) {
        child->render(shader, ctx);
    }
}

