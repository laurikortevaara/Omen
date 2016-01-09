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
        m_yaw(0), m_pitch(0), m_roll(0),
        m_lookAt(lookAt),
        m_fov(fov),
        m_bIsValid(false),
        m_joystick(nullptr),
        m_direction(glm::normalize(lookAt - pos)) {
    m_view = glm::lookAt(pos, lookAt, glm::vec3(0, 1, 0));

    m_right = glm::vec3(m_view[0]);
    m_up = glm::vec3(m_view[1]);
    m_forward = glm::vec3(m_view[2]);

    // Connect key-hit, -press and -release signals to observers
    Omen::Engine *e = Omen::Engine::instance();
    Omen::Window *w = e->window();

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

            m_yaw += dx;
            m_pitch -= dy;
        });
    }

    e->signal_engine_update.connect([this, e, w](double time, double deltaTime) {
        // velocity = velocity + accelleration
        // velo = m/s
        // acceleration = m/s^2
        m_acceleration = 0.25;

        if (w->keyPressed(GLFW_KEY_W)) {
            m_velocity.z += m_acceleration * deltaTime;
        }
        if (w->keyPressed(GLFW_KEY_S)) {
            m_velocity.z -= m_acceleration * deltaTime;
        }

        if (w->keyPressed(GLFW_KEY_A)) {
            m_velocity.x -= m_acceleration * deltaTime;
        }
        if (w->keyPressed(GLFW_KEY_D)) {
            m_velocity.x += m_acceleration * deltaTime;
        }

        if (w->keyPressed(GLFW_KEY_E)) {
            m_velocity.y += m_acceleration * deltaTime;
        }
        if (w->keyPressed(GLFW_KEY_C)) {
            m_velocity.y -= m_acceleration * deltaTime;
        }

        if (m_joystick != nullptr) {
            std::vector<float> &axes = m_joystick->getJoystickAxes();
            std::vector<int> &buttons = m_joystick->getJoystickButtons();

            bool valid = true;
            if(axes.size() >=4 && axes[0]==-1 && axes[1]  == -1 && axes[2]==-1 && axes[3] == -1)
                valid = false;
            if(valid){
                float dx = axes[2];
                float dy = axes[3];
                m_yaw += 200.0f * dx * deltaTime;
                m_pitch -= 200.0f * dy * deltaTime;
                m_velocity.x += axes[0] * deltaTime;
                m_velocity.z -= axes[1] * deltaTime;
            }

        }

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, m_direction));
        glm::vec3 cameraUp = glm::cross(m_direction, cameraRight);
        glm::vec3 cameraFront = glm::cross(cameraRight, cameraUp);

        m_pitch = fmax(m_pitch, -89.9);
        m_pitch = fmin(m_pitch, 89.9);

        m_direction = glm::normalize(glm::vec3(cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw)),
                                sin(glm::radians(m_pitch)),
                                cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw))));

        m_view = glm::lookAt(m_pos, m_pos+m_direction, glm::vec3(0, 1, 0));

        m_pos += cameraFront * m_velocity.z;
        m_pos += glm::normalize(glm::cross(cameraFront, cameraUp))*m_velocity.x;
        m_pos += glm::normalize(glm::cross(cameraFront, cameraRight))*m_velocity.y;

        m_velocity *= 0.95;

        updateMVP();
    });
}

void Camera::onWindowSizeChanged(int width, int height) {
    m_bIsValid = false;
}

/**
 * Return ModelViewProjection Matrix
 */
glm::mat4x4 &Camera::mvp() {
    if (!m_bIsValid) {
        updateMVP();
    }
    return m_mvp;
}

/**
 * Return ModelViewProjection Matrix
 */
glm::mat4x4 &Camera::viewMatrix() {
    return m_view;
}

void Camera::updateMVP() {
    double ViewPortParams[4];
    glGetDoublev(GL_VIEWPORT, ViewPortParams);
    GLFWwindow *pWindow = glfwGetCurrentContext();

    int width, height;
    glfwGetWindowSize(pWindow, &width, &height);
    float aspectRatio = width / (float) height;
    // Generates a really hard-to-read matrix, but a normal, standard 4x4 matrix nonetheless
    glm::mat4 Projection = glm::perspective(
            45.0f,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
            aspectRatio, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
            0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
            1000.0f       // Far clipping plane. Keep as little as possible.
    );

    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 model(1);
    model = glm::translate(model, -m_pos);
    m_mvp = Projection * m_view * model;     // Remember, matrix multiplication is the other way around
    m_bIsValid = true;
}
