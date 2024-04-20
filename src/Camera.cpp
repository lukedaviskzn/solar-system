#include "Camera.h"

#include <iostream>
#include <SDL2/SDL.h>

Camera::Camera(int camera_id, Transform transform, float fovy) : active(true), camera_id(camera_id), transform(transform), perspective(1.0), aspect(1.0), fovy(fovy) {}

void Camera::update(UpdateContext& ctx) {
    active = camera_id == ctx.active_camera;
    
    if (ctx.aspect != aspect) {
        aspect = ctx.aspect;
        perspective = glm::perspective(fovy, aspect, 0.01f, 100000.0f);
    }

    if (!ctx.grabbed || !active) return;

    double speed = ctx.dt * 0.1;

    glm::dvec3 dir(0.0);
    
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

void Camera::prerender(Shader& shader, UpdateContext& ctx) {
    if (!active) return;
    
    shader.setUniform("view", glm::inverse(transform.globalMatrix));
    shader.setUniform("perspective", perspective);

    ctx.camera_pos = glm::dvec3(transform.globalMatrix * glm::dvec4(0.0, 0.0, 0.0, 1.0));
}
