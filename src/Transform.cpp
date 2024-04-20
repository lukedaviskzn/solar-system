#include "Transform.h"
#include <iostream>

Transform::Transform():
    position(glm::vec3(0.0)), rotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f))),
    scale(glm::vec3(1.0)), localMatrix(1.0), globalMatrix(1.0) {}

Transform::Transform(glm::vec3 position):
    position(position), rotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f))),
    scale(glm::vec3(1.0)), localMatrix(1.0), globalMatrix(1.0) {}

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale):
    position(position), rotation(rotation), scale(scale),
    localMatrix(1.0), globalMatrix(1.0) {}

void Transform::computeMatrix() {
    localMatrix = glm::mat4(1.0);
    localMatrix = glm::translate(localMatrix, position);
    localMatrix *= glm::mat4_cast(rotation);
    localMatrix = glm::scale(localMatrix, scale);
}

void Transform::computeGlobalMatrix(const glm::mat4x4& parent) {
    globalMatrix = parent * localMatrix;
}
