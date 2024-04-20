#pragma once

#include <glm/glm.hpp>
#include "GameObject.h"
#include "Mesh.h"
#include "Transform.h"

/// @brief A scene object has no logic, but has children.
class Scene: public GameObject
{
private:
    std::vector<GameObject*> children;
public:
    Scene(std::vector<GameObject*> children);
    Scene(const Scene& _) = delete;
    void operator=(const Scene& _) = delete;
    ~Scene();
    
    void update(UpdateContext& ctx);
    void physics_update(UpdateContext& ctx);
    void update_transform(const glm::mat4& parent);
    void prerender(Shader& shader);
    void render(Shader& shader);
};
