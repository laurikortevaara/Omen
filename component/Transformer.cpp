//
// Created by Lauri Kortevaara(Intopalo) on 10/01/16.
//

#include "Transformer.h"
#include "../system/InputSystem.h"
#include "MouseInput.h"
#include "../Engine.h"
#include "Transformer.h"
#include "Picker.h"
#include "KeyboardInput.h"

using namespace omen;

Transformer::Transformer() : m_tr(nullptr), m_joystick(nullptr) {

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
        picker->signal_object_picked.connect([&](omen::Mesh* obj){
            setEnabled(obj!= nullptr);
            m_obj = obj;
            if(obj) {
                m_tr = &obj->m_transform;
            }
            else
                m_tr = nullptr;
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

            /*if (m_camera >= nullptr) {
                m_camera->yaw() += -dx;
                m_camera->pitch() += dy;
            }*/

        });
    }

    Engine::instance()->signal_engine_update.connect([this](double time, double deltaTime) {
        if(!enabled()||m_tr == nullptr)
            return;

        // velocity = velocity + accelleration
        // velo = m/s
        // acceleration = m/s^2
        Engine* e = Engine::instance();
        KeyboardInput* ki = e->findComponent<KeyboardInput>();

        if (m_tr != nullptr) {
            deltaTime *= 100.0f;
            if (ki->keyPressed(GLFW_KEY_W)) {
                //m_obj->m_transform.pos().z += deltaTime;
                m_obj->m_transform.translate(glm::vec3(0,0,deltaTime));
            }
            if (ki->keyPressed(GLFW_KEY_S)) {
                //m_obj->m_transform.pos().z -= deltaTime;
                m_obj->m_transform.translate(glm::vec3(0,0,-deltaTime));
            }

            if (ki->keyPressed(GLFW_KEY_A)) {
                //m_obj->m_transform.pos().x -= deltaTime;
                m_obj->m_transform.translate(glm::vec3(-deltaTime,0,0));
            }
            if (ki->keyPressed(GLFW_KEY_D)) {
                //m_obj->m_transform.pos().x += deltaTime;
                m_obj->m_transform.translate(glm::vec3(deltaTime,0,0));
            }

            if (ki->keyPressed(GLFW_KEY_E)) {
                m_obj->m_transform.pos().y += deltaTime;
            }
            if (ki->keyPressed(GLFW_KEY_C)) {
                m_obj->m_transform.pos().y -= deltaTime;
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
                    /*m_camera->yaw() += 200.0f * dx * deltaTime;
                    m_camera->pitch() -= 200.0f * dy * deltaTime;
                    m_camera->velocity().x += axes[0] * deltaTime;
                    m_camera->velocity().z -= axes[1] * deltaTime;
                     */
                }

            }
        }
    });
}

Transformer::~Transformer() {

}

void Transformer::onAttach(ecs::Entity *e) {
    m_entity = e;
}

void Transformer::onDetach(ecs::Entity *e) {
    m_entity = nullptr;
}
