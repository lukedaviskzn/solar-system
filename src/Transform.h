#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    glm::mat4x4 localMatrix;
    glm::mat4x4 globalMatrix;
    
    Transform();
    Transform(glm::vec3 position);
    Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

    void computeMatrix();
    void computeGlobalMatrix(const glm::mat4x4& parent);
};
