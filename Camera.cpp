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

using namespace Omen;

/**
 *
 */
Camera::Camera(const std::string &name, const glm::vec3 &pos, const ::glm::vec3 &lookAt, float fov) :
        GameObject(name),
        m_pos(pos),
        m_near(0.01), m_far(100.0),
        m_yaw(0), m_pitch(0), m_roll(0),
        m_lookAt(lookAt),
        m_fov(fov*glm::pi<float>()/180.0f),
        m_bIsValid(false),
        m_joystick(nullptr) {

    Omen::Engine *e = Omen::Engine::instance();
    Omen::Window *w = e->window();

    // Create View matrix
    float aspect = (float) w->width() / (float) w->height();
    m_view = glm::perspective(m_fov, aspect, m_near, m_far);

    // Initialize forward and up vectors
    m_forward = glm::vec3(0, 0, 1);
    m_up = glm::vec3(0, 1, 0);


    // Connect key-hit, -press and -release signals to observers
    JoystickInput *ji = e->findSystem<InputSystem>()->findComponent<JoystickInput>();
    if (ji != nullptr) {
        ji->joystick_connected.connect([&](Joystick *joystick) {
            m_joystick = joystick;
        });
        ji->joystick_disconnected.connect([&](Joystick *joystick) {
            m_joystick = nullptr;
        });
    }

    MouseInput *mi = e->findSystem<InputSystem>()->findComponent<MouseInput>();
    if (mi != nullptr) {
        mi->signal_cursorpos_changed.connect([&](double x, double y) {
            static double old_x = x;
            static double old_y = y;
            double dx = x - old_x;
            double dy = y - old_y;
            old_x = x;
            old_y = y;

            m_yaw -= dx;
            m_pitch += dy;
        });
    }

    e->signal_engine_update.connect([this, e, w](double time, double deltaTime) {
        // velocity = velocity + accelleration
        // velo = m/s
        // acceleration = m/s^2
        m_acceleration = glm::vec3(0.35);

        if (w->keyPressed(GLFW_KEY_W)) {
            m_velocity.z += m_acceleration.z * deltaTime;
        }
        if (w->keyPressed(GLFW_KEY_S)) {
            m_velocity.z -= m_acceleration.z * deltaTime;
        }

        if (w->keyPressed(GLFW_KEY_A)) {
            m_velocity.x -= m_acceleration.x * deltaTime;
        }
        if (w->keyPressed(GLFW_KEY_D)) {
            m_velocity.x += m_acceleration.x * deltaTime;
        }

        if (w->keyPressed(GLFW_KEY_E)) {
            m_velocity.y += m_acceleration.y * deltaTime;
        }
        if (w->keyPressed(GLFW_KEY_C)) {
            m_velocity.y -= m_acceleration.y * deltaTime;
        }

        if (m_joystick != nullptr) {
            std::vector<float> &axes = m_joystick->getJoystickAxes();
            std::vector<int> &buttons = m_joystick->getJoystickButtons();

            bool valid = true;
            if (axes.size() >= 4 && axes[0] == -1 && axes[1] == -1 && axes[2] == -1 && axes[3] == -1)
                valid = false;
            if (valid) {
                float dx = axes[2];
                float dy = axes[3];
                m_yaw += 200.0f * dx * deltaTime;
                m_pitch -= 200.0f * dy * deltaTime;
                m_velocity.x += axes[0] * deltaTime;
                m_velocity.z -= axes[1] * deltaTime;
            }

        }
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

        m_view = glm::lookAt(m_pos, m_pos+m_forward, m_up);
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
 * Return ModelViewProjection Matrix
 */
glm::mat4x4 &Camera::view() {
    return m_view;
}

void Camera::updateViewProjection() {

    Omen::Engine *e = Omen::Engine::instance();
    Omen::Window *w = e->window();

    float aspectRatio = (float) w->width() / (float) w->height();
    // Generates a really hard-to-read matrix, but a normal, standard 4x4 matrix nonetheless
    glm::mat4 Projection = glm::perspective(
            m_fov,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
            aspectRatio, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
            m_near,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
            m_far       // Far clipping plane. Keep as little as possible.
    );

    // Our ModelViewProjection : multiplication of our 3 matrices
    m_viewProj = Projection * m_view;     // Remember, matrix multiplication is the other way around
    m_bIsValid = true;
}

void Camera::setPosition(glm::vec3 position) {
    m_pos = position;
}
