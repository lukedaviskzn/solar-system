#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "GameObject.h"
#include "Mesh.h"
#include "Transform.h"

class Orbit: public GameObject
{
private:
    std::vector<GameObject*> children;
    Transform transform;
    float time;
    double orbitRadius;
    double speed;
public:
    Orbit(double orbitRadius, double speed, std::vector<GameObject*> children);
    Orbit(const Orbit& _) = delete;
    void operator=(const Orbit& _) = delete;
    ~Orbit();
    
    void update(UpdateContext& ctx);
    void physics_update(UpdateContext& ctx);
    void update_transform(const glm::mat4& parent);
    void prerender(Shader& shader, UpdateContext& ctx);
    void render(Shader& shader, UpdateContext& ctx);
};
