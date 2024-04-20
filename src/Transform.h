#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
    glm::dvec3 position;
    glm::dquat rotation;
    glm::dvec3 scale;
    glm::dmat4x4 localMatrix;
    glm::dmat4x4 globalMatrix;
    
    Transform();
    Transform(glm::dvec3 position);
    Transform(glm::dvec3 position, glm::dquat rotation, glm::dvec3 scale);

    void computeMatrix();
    void computeGlobalMatrix(const glm::dmat4x4& parent);
};
