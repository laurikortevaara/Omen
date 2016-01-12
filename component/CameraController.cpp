//
// Created by Lauri Kortevaara(Intopalo) on 10/01/16.
//

#include <CoreImage/CoreImage.h>
#include "CameraController.h"
#include "../system/InputSystem.h"
#include "MouseInput.h"
#include "../Engine.h"
#include "Picker.h"

Omen::CameraController::CameraController() : m_camera(nullptr), m_joystick(nullptr) {

    // Get the connected joystic
    JoystickInput *ji = Engine::instance()->findComponent<JoystickInput>();
    if (ji != nullptr) {
        ji->joystick_connected.connect([&](Joystick *joystick) {
            m_joystick = joystick;
        });
        ji->joystick_disconnected.connect([&](Joystick *joystick) {
            m_joystick = nullptr;
        });
    }

    Picker* picker = Engine::instance()->findComponent<Picker>();
    if(picker)
        picker->signal_object_picked.connect([&](Omen::Mesh* obj){
           this->setEnabled(obj== nullptr);
        });

    // Get the Mouse coordinates
    MouseInput *mi = Engine::instance()->findComponent<MouseInput>();
    if (mi != nullptr) {
        mi->signal_cursorpos_changed.connect([&](double x, double y) {
            if(!enabled())
                return;
            static double old_x = x;
            static double old_y = y;
            double dx = x - old_x;
            double dy = y - old_y;
            old_x = x;
            old_y = y;

            if (m_camera >= nullptr) {
                m_camera->yaw() += -dx;
                m_camera->pitch() += dy;
            }

        });
    }

    Engine::instance()->signal_engine_update.connect([this](double time, double deltaTime) {
        if(!enabled())
            return;

        // velocity = velocity + accelleration
        // velo = m/s
        // acceleration = m/s^2
        Engine* e = Engine::instance();
        Window* w = e->window();

        if (m_camera != nullptr) {
            m_camera->acceleration() = glm::vec3(0.35);

            if (w->keyPressed(GLFW_KEY_W)) {
                m_camera->velocity().z += m_camera->acceleration().z * deltaTime;
            }
            if (w->keyPressed(GLFW_KEY_S)) {
                m_camera->velocity().z -= m_camera->acceleration().z * deltaTime;
            }

            if (w->keyPressed(GLFW_KEY_A)) {
                m_camera->velocity().x -= m_camera->acceleration().x * deltaTime;
            }
            if (w->keyPressed(GLFW_KEY_D)) {
                m_camera->velocity().x += m_camera->acceleration().x * deltaTime;
            }

            if (w->keyPressed(GLFW_KEY_E)) {
                m_camera->velocity().y += m_camera->acceleration().y * deltaTime;
            }
            if (w->keyPressed(GLFW_KEY_C)) {
                m_camera->velocity().y -= m_camera->acceleration().y * deltaTime;
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
                    m_camera->yaw() += 200.0f * dx * deltaTime;
                    m_camera->pitch() -= 200.0f * dy * deltaTime;
                    m_camera->velocity().x += axes[0] * deltaTime;
                    m_camera->velocity().z -= axes[1] * deltaTime;
                }

            }
        }
    });
}

Omen::CameraController::~CameraController() {

}