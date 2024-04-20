#pragma once

#include <unordered_map>
#include <SDL2/SDL.h>

struct UpdateContext {
    float dt;
    float aspect;
    std::unordered_map<int32_t, bool> keys_pressed;
    int32_t mouse_dx, mouse_dy;
    bool grabbed;
};
