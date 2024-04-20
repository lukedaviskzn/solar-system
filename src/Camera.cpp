#include "Camera.h"

#include <iostream>
#include <SDL2/SDL.h>

Camera::Camera(Transform transform, float fovy) : transform(transform), perspective(glm::perspective(fovy, 1.0f, 0.01f, 2000.0f)), aspect(1.0), fovy(fovy) {}

void Camera::update(UpdateContext& ctx) {
    if (ctx.aspect != aspect) {
        aspect = ctx.aspect;
        perspective = glm::perspective(fovy, aspect, 0.01f, 2000.0f);
    }

    if (!ctx.grabbed) return;

    float speed = ctx.dt * 0.1f;

    glm::vec3 dir = glm::vec3(0.0f);
    
    if (ctx.keys_pressed[SDLK_w]) {
        dir += glm::vec3(0.0f, 0.0f, -1.0f);
    }
    if (ctx.keys_pressed[SDLK_s]) {
        dir += glm::vec3(0.0f, 0.0f, 1.0f);
    }
    if (ctx.keys_pressed[SDLK_a]) {
        dir += glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    if (ctx.keys_pressed[SDLK_d]) {
        dir += glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (ctx.keys_pressed[SDLK_SPACE]) {
        dir += glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (ctx.keys_pressed[SDLK_LCTRL]) {
        dir += glm::vec3(0.0f, -1.0f, 0.0f);
    }
    
    if (glm::length(dir) > 0.1f) {
        dir = speed * glm::normalize(dir);
    }

    dir = transform.rotation * dir;

    float rotSpeed = ctx.dt * 0.1f;
    
    transform.rotation *= glm::quat(-ctx.mouse_dx * rotSpeed * glm::vec3(0.0f, 1.0f, 0.0f));
    transform.rotation *= glm::quat(-ctx.mouse_dy * rotSpeed * glm::vec3(1.0f, 0.0f, 0.0f));
    
    // remove roll
    // glm::vec3 euler = glm::eulerAngles(transform.rotation);
    // transform.rotation = glm::quat(glm::vec3(euler.x, euler.y, 0.0f));

    transform.position += dir;
}

void Camera::update_transform(const glm::mat4& parent) {
    transform.computeMatrix();
    transform.computeGlobalMatrix(parent);
}

void Camera::prerender(Shader& shader) {
    shader.setUniform("view", glm::inverse(transform.globalMatrix));
    shader.setUniform("perspective", perspective);
}
