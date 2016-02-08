//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include <GLFW/glfw3.h>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ostream>
#include <iostream>
#include "Camera.h"
#include "Engine.h"
#include <math.h>
#include "component/JoystickInput.h"
#include "system/InputSystem.h"
#include "MathUtils.h"
#include "component/MouseInput.h"

using namespace omen;

/**
 *
 */
Camera::Camera(const std::string &name, const glm::vec3 &pos, const ::glm::vec3 &lookAt, float fov) :
        GameObject(name),
        m_pos(pos),
        m_near(0.01), m_far(1000.0),
        m_yaw(0), m_pitch(0), m_roll(0),
        m_lookAt(lookAt),
        m_fov(fov*glm::pi<float>()/180.0f),
        m_bIsValid(false),
        m_joystick(nullptr) {

    omen::Engine *e = omen::Engine::instance();
    omen::Window *w = e->window();

    // Create View matrix
    float aspect = (float) w->width() / (float) w->height();
    m_view = glm::perspective(m_fov, aspect, m_near, m_far);

    m_pitch = 30;

    // Initialize forward and up vectors
    m_forward = glm::vec3(0, 0, 1);
    m_up = glm::vec3(0, 1, 0);
    m_pos.z *= -1;

    // connect to engine update signal
    Engine::instance()->signal_engine_update.connect([this](double time, double deltaTime){
        glm::mat4 m = glm::rotate(glm::mat4(1), m_yaw*glm::pi<float>()/180.0f, glm::vec3(0,1,0));
                  m = glm::rotate(m, m_pitch*glm::pi<float>()/180.0f, glm::vec3(1,0,0));
        m_forward = glm::vec3(m * glm::vec4(0,0,1,0));

        glm::vec3 cameraRight = glm::normalize(glm::cross(m_up, m_forward));
        glm::vec3 cameraUp = glm::cross(m_forward, cameraRight);
        glm::vec3 cameraFront = glm::cross(cameraRight, cameraUp);

        m_pitch = fmax(m_pitch, -89.9);
        m_pitch = fmin(m_pitch, 89.9);
        m_pos += m_forward * m_velocity.z;
        m_pos += glm::normalize(glm::cross(m_forward, m_up))*m_velocity.x;
        m_pos += glm::normalize(glm::cross(m_forward, cameraRight))*m_velocity.y;

        m_view = glm::scale(glm::lookAt(m_pos, m_pos+m_forward, m_up), {-1,1,1});
        m_velocity *= 0.90;

        updateViewProjection();
    });
}

void Camera::onWindowSizeChanged(int width, int height) {
    m_bIsValid = false;
}

/**
 * Return ModelViewProjection Matrix
 */
glm::mat4x4 &Camera::viewProjection() {
    if (!m_bIsValid) {
        updateViewProjection();
    }
    return m_viewProj;
}

/**
 * Return View Matrix
 */
glm::mat4x4 &Camera::view() {
    return m_view;
}

/**
 * Return Projection Matrix
 */
glm::mat4x4 &Camera::projection() {
    return m_projection;
}


void Camera::updateViewProjection() {

    omen::Engine *e = omen::Engine::instance();
    omen::Window *w = e->window();

    float aspectRatio = (float) w->width() / (float) w->height();
    // Generates a really hard-to-read matrix, but a normal, standard 4x4 matrix nonetheless
    m_projection = glm::perspective(
            m_fov,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
            aspectRatio, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
            m_near,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
            m_far       // Far clipping plane. Keep as little as possible.
    );

    // Our ModelViewProjection : multiplication of our 3 matrices
    m_viewProj = m_projection * m_view;     // Remember, matrix multiplication is the other way around
    m_bIsValid = true;
}

void Camera::setPosition(glm::vec3 position) {
    m_pos = position;
}

float& Camera::yaw() {
    return m_yaw;
}

float& Camera::pitch() {
    return m_pitch;
}

glm::vec3& Camera::acceleration() {
    return m_acceleration;
}

glm::vec3& Camera::velocity() {
    return m_velocity;
}