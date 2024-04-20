#pragma once

#include "GameObject.h"
#include "Transform.h"

class Camera: public GameObject
{
private:
    Transform transform;
    glm::mat4 perspective;
    float aspect;
    float fovy;
public:
    Camera(Transform transform, float fovy);
    
    void update(UpdateContext& ctx);
    void update_transform(const glm::mat4& parent);
    void prerender(Shader& shader);
};
