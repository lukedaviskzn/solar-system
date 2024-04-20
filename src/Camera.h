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
    int camera_id;
    bool active;
public:
    Camera(int camera_id, Transform transform, float fovy);
    
    void update(UpdateContext& ctx);
    void update_transform(const glm::mat4& parent);
    void prerender(Shader& shader, UpdateContext& ctx);
};
