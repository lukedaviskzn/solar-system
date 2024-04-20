#pragma once

#include <unordered_map>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct UpdateContext {
    float dt;
    float aspect;
    std::unordered_map<int32_t, bool> keys_pressed;
    int32_t mouse_dx, mouse_dy;
    bool grabbed;
    int active_camera;
    glm::dvec3 camera_pos;
};
