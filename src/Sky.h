#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"

class Sky: public GameObject
{
private:
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Mesh> mesh;
    float scale;
public:
    Sky(std::shared_ptr<Texture> texture, std::shared_ptr<Mesh> mesh, float scale);
    
    void render(Shader& shader, UpdateContext& ctx);
};
