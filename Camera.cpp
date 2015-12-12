//
// Created by Lauri Kortevaara(personal) on 28/11/15.
//

#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

Camera::Camera() {velocity={0,0,0}; }

Camera::~Camera() { }

void Camera::update() {
    position += velocity;
    velocity *= 0.01;
    view = glm::lookAt(position, lookAt, glm::vec3(0, 1, 0));
    projection = glm::perspective(glm::radians(viewAngle), (float) 4 / (float) 3, near, far);
}
