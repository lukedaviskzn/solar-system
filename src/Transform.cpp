#include "Transform.h"
#include <iostream>

Transform::Transform():
    position(0.0), rotation(glm::dvec3(0.0, 0.0, 0.0)),
    scale(1.0), localMatrix(1.0), globalMatrix(1.0) {}

Transform::Transform(glm::dvec3 position):
    position(position), rotation(glm::dvec3(0.0, 0.0, 0.0)),
    scale(1.0), localMatrix(1.0), globalMatrix(1.0) {}

Transform::Transform(glm::dvec3 position, glm::dquat rotation, glm::dvec3 scale):
    position(position), rotation(rotation), scale(scale),
    localMatrix(1.0f), globalMatrix(1.0f) {}

void Transform::computeMatrix() {
    localMatrix = glm::dmat4(1.0);
    localMatrix = glm::translate(localMatrix, position);
    localMatrix *= glm::mat4_cast(rotation);
    localMatrix = glm::scale(localMatrix, scale);
}

void Transform::computeGlobalMatrix(const glm::dmat4x4& parent) {
    globalMatrix = parent * localMatrix;
}
