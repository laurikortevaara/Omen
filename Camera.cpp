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

using namespace Omen;

/**
 *
 */
Camera::Camera(const std::string &name, const glm::vec3 &pos, const ::glm::vec3 &lookAt, float fov) :
        GameObject(name),
        m_pos(pos),
        m_lookAt(lookAt),
        m_fov(fov),
        m_bIsValid(false),
        m_joystick(nullptr) {
    m_view = glm::lookAt(pos, lookAt, glm::vec3(0, 1, 0));
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

    e->signal_engine_update.connect([this, e, w](double time, double deltaTime) {
        // velocity = velocity + accelleration
        // velo = m/s
        // acceleration = m/s^2

        if (w->keyPressed(GLFW_KEY_W)) {
            m_velocity.z += 100.0 * deltaTime;
        }
        if (w->keyPressed(GLFW_KEY_S)) {
            m_velocity.z -= 100.0 * deltaTime;
        }

        if (w->keyPressed(GLFW_KEY_A)) {
            m_velocity.x = 1.0 * deltaTime;
        }
        if (w->keyPressed(GLFW_KEY_D)) {
            m_velocity.x = 1.0 * deltaTime;
        }

        if (m_joystick != nullptr) {
            std::vector<float> &axes = m_joystick->getJoystickAxes();
            std::vector<int> &buttons = m_joystick->getJoystickButtons();

            if (axes.size() >= 4) {
                m_velocity.x = (float) (axes[Joystick::X_AXIS] * (buttons[Joystick::R2] ? 100 : 1.0));
                m_velocity.y = (float) (axes[Joystick::Y_AXIS] * (buttons[Joystick::R2] ? 100 : 1.0));
                m_velocity.z = (float) (axes[Joystick::Z_AXIS] * (buttons[Joystick::R2] ? 100 : 1.0));
            }

        }

        glm::vec3 up(0, 1, 0);
        glm::vec3 forward = glm::normalize(m_lookAt - m_pos);
        glm::vec3 right = glm::cross(up, forward);

        /*
        std::cout   << "F:(" << forward.x << "," << forward.y << "," << forward.z << std::endl\
                    << "R:(" << right.x << "," << right.y << "," << right.z << std::endl\
                    << "U:(" << up.x << "," << up.y << "," << up.z << std::endl;
        */
        m_bIsValid &= m_velocity.length() == 0.0f;
        m_view = glm::translate(m_view, -m_velocity * (float) deltaTime);
        m_pos = glm::vec3(m_view[3]);

        m_velocity *= 0.9;
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
        double ViewPortParams[4];
        glGetDoublev(GL_VIEWPORT, ViewPortParams);
        GLFWwindow *pWindow = glfwGetCurrentContext();

        int width, height;
        glfwGetWindowSize(pWindow, &width, &height);
        float aspectRatio = width / (float) height;
        // Generates a really hard-to-read matrix, but a normal, standard 4x4 matrix nonetheless
        glm::mat4 Projection = glm::perspective(
                m_fov,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
                aspectRatio, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
                0.0f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
                100.0f       // Far clipping plane. Keep as little as possible.
        );

        // Our ModelViewProjection : multiplication of our 3 matrices
        m_mvp = Projection * m_view; // Remember, matrix multiplication is the other way around
        m_bIsValid = true;
    }
    return m_mvp;
}