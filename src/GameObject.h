#pragma once

#include "Shader.h"
#include "UpdateContext.h"

class GameObject
{
public:
    virtual void update(UpdateContext& ctx) {}
    virtual void physics_update(UpdateContext& ctx) {}
    virtual void update_transform(const glm::mat4& parent) {}
    virtual void prerender(Shader& shader, UpdateContext& ctx) {}
    virtual void render(Shader& shader, UpdateContext& ctx) {}
};
