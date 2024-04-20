#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"

class StellarBody: public GameObject
{
private:
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Texture> texture_night;
    std::shared_ptr<Mesh> mesh;
    Transform transform;
    float rotSpeed;
    glm::vec3 rotAxis;
    bool has_night;
    int lighting;
public:
    StellarBody(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> texture_night, std::shared_ptr<Mesh> mesh, float scale, float rotSpeed, float rotAngle);
    StellarBody(std::shared_ptr<Texture> texture, int lighting, std::shared_ptr<Mesh> mesh, float scale, float rotSpeed, float rotAngle);
    
    void physics_update(UpdateContext& ctx);
    void update_transform(const glm::mat4& parent);
    void render(Shader& shader);
};
