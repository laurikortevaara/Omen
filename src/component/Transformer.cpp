//
// Created by Lauri Kortevaara on 10/01/16.
//

#include "component/Transformer.h"

#include "Engine.h"
#include "GameObject.h"

#include "system/InputSystem.h"
#include "component/MouseInput.h"
#include "component/Transformer.h"
#include "component/Picker.h"
#include "component/KeyboardInput.h"


using namespace omen;

Transformer::Transformer() : m_tr(nullptr), m_joystick(nullptr) {

    // Get the connected joystic
    JoystickInput *ji = Engine::instance()->findComponent<JoystickInput>();
    if (ji != nullptr) {
        ji->joystick_connected.connect(this,[&](Joystick *joystick) {
            m_joystick = joystick;
        });
        ji->joystick_disconnected.connect(this,[&](Joystick *joystick) {
            m_joystick = nullptr;
        });
    }

    Picker* picker = Engine::instance()->findComponent<Picker>();
    if(picker)
		picker->signal_object_picked.connect(this,[&](ecs::Entity* obj, glm::vec3 intersectPos) 
		{
			setEnabled(obj != nullptr);
			if (obj) {
				m_obj = dynamic_cast<omen::ecs::GameObject*>(obj);
				if (obj) {
					m_tr = m_obj->tr();
				}
				else
					m_tr = nullptr;
		}
        });

    // Get the Mouse coordinates
    MouseInput *mi = Engine::instance()->findComponent<MouseInput>();
    if (mi != nullptr) {
        mi->signal_cursorpos_changed.connect(this,[&](double x, double y) {
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

    Engine::instance()->signal_engine_update.connect(this,[this](double time, double deltaTime) {
        if(!enabled()||m_tr == nullptr)
            return;

        // velocity = velocity + accelleration
        // velo = m/s
        // acceleration = m/s^2
        Engine* e = Engine::instance();
        KeyboardInput* ki = e->findComponent<KeyboardInput>();

		if (ki->keyPressed(GLFW_KEY_ESCAPE)) {
			setEnabled(false);
		}

        if (m_tr != nullptr) {
            deltaTime *= 100.0f;
            if (ki->keyPressed(GLFW_KEY_W)) {
                //m_obj->tr()->pos().z += deltaTime;
                m_obj->tr()->translate(glm::vec3(0,0,deltaTime));
            }
            if (ki->keyPressed(GLFW_KEY_S)) {
                //m_obj->tr()->pos().z -= deltaTime;
                m_obj->tr()->translate(glm::vec3(0,0,-deltaTime));
            }

            if (ki->keyPressed(GLFW_KEY_A)) {
                //m_obj->tr()->pos().x -= deltaTime;
                m_obj->tr()->translate(glm::vec3(-deltaTime,0,0));
            }
            if (ki->keyPressed(GLFW_KEY_D)) {
                //m_obj->tr()->pos().x += deltaTime;
                m_obj->tr()->translate(glm::vec3(deltaTime,0,0));
            }

            if (ki->keyPressed(GLFW_KEY_E)) {
				m_obj->tr()->translate(glm::vec3(0, deltaTime, 0));
            }
            if (ki->keyPressed(GLFW_KEY_C)) {
				m_obj->tr()->translate(glm::vec3(0, -deltaTime, 0));
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
