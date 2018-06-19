//
// Created by Lauri Kortevaara on 10/01/16.
//
#include "component/CameraController.h"

#include "Engine.h"
#include "system/InputSystem.h"
#include "component/MouseInput.h"
#include "component/Picker.h"
#include "component/KeyboardInput.h"

omen::CameraController::CameraController() : m_camera(nullptr), m_joystick(nullptr) {

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
	/*if (picker)
		picker->signal_object_picked.connect(this,[&](ecs::Entity* obj, glm::vec3 intersectPos) {
		this->setEnabled(obj == nullptr);
	});*/

	setEnabled(false);

	// Get the Mouse coordinates
	MouseInput *mi = Engine::instance()->findComponent<MouseInput>();
	if (mi != nullptr) {
		mi->signal_cursorpos_changed.connect(this,[&](omen::floatprec x, omen::floatprec y) {
			static omen::floatprec old_x = x;
			static omen::floatprec old_y = y;
			omen::floatprec dx = x - old_x;
			omen::floatprec dy = y - old_y;
			old_x = x;
			old_y = y;

			if (!enabled())
				return;

			if (m_camera >= nullptr) {
				m_camera->yaw() += -dx*Engine::instance()->CameraSensitivity;
				m_camera->pitch() += dy*Engine::instance()->CameraSensitivity;
			}

		});
	}

	Engine::instance()->signal_engine_update.connect(this,[this](omen::floatprec time, omen::floatprec deltaTime) {
		Engine* e = Engine::instance();
		KeyboardInput* ki = e->findComponent<KeyboardInput>();
		if (ki->keyPressed(GLFW_KEY_P)) {
			setEnabled(!enabled());
		}

		if (!enabled())
			return;

		// velocity = velocity + accelleration
		// velo = m/s
		// acceleration = m/s^2
		if (m_camera != nullptr && !ki->keyModifierPressed(GLFW_MOD_SHIFT)) {
			m_camera->acceleration() = glm::vec3(10.35f);

			float coeff = 0.1f;
			if (ki->keyPressed(GLFW_KEY_W)) {
				m_camera->velocity().z += m_camera->acceleration().z * deltaTime*coeff;
			}
			if (ki->keyPressed(GLFW_KEY_S)) {
				m_camera->velocity().z -= m_camera->acceleration().z * deltaTime*coeff;
			}

			if (ki->keyPressed(GLFW_KEY_A)) {
				m_camera->velocity().x -= m_camera->acceleration().x * deltaTime*coeff;
			}
			if (ki->keyPressed(GLFW_KEY_D)) {
				m_camera->velocity().x += m_camera->acceleration().x * deltaTime*coeff;
			}

			if (ki->keyPressed(GLFW_KEY_E)) {
				m_camera->velocity().y += m_camera->acceleration().y * deltaTime*coeff;
			}
			if (ki->keyPressed(GLFW_KEY_C)) {
				m_camera->velocity().y -= m_camera->acceleration().y * deltaTime*coeff;
			}

			if (m_joystick != nullptr) {
				std::vector<float> &axes = m_joystick->getJoystickAxes();
				std::vector<int> &buttons = m_joystick->getJoystickButtons();

				bool valid = true;
				if (axes.size() >= 4 && axes[0] == -1 && axes[1] == -1 && axes[2] == -1 && axes[3] == -1)
					valid = false;
				if (valid) {
					omen::floatprec dx = axes[2];
					omen::floatprec dy = axes[3];
					m_camera->yaw() += 200.0f * dx * deltaTime;
					m_camera->pitch() -= 200.0f * dy * deltaTime;
					m_camera->velocity().x += axes[0] * deltaTime;
					m_camera->velocity().z -= axes[1] * deltaTime;
				}
			}
		}
	});
}

omen::CameraController::~CameraController() {

}

void omen::CameraController::onAttach(omen::ecs::Entity *e) {
	m_entity = e;
}


void omen::CameraController::onDetach(omen::ecs::Entity *e) {
	m_entity = nullptr;
}
