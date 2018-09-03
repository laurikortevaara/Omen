//
// Created by Lauri Kortevaara on 26/12/15.
//


#include "component/JoystickInput.h"

#include "Window.h"
#include "Engine.h"
#include "MathUtils.h"

#include <glfw/glfw3.h>

#include <ostream>
#include <iostream>

using namespace omen;

JoystickInput::JoystickInput() : Component() {
    // KeyHit signal handler
    // Add a static C-function callback wrapper with pointer to this
    m_window = glfwGetCurrentContext();

	for (int i = 0; i < Joystick::MAX_JOYSTICK_COUNT; ++i) {
		if (Joystick::isPresent(i)) {
			addJoystick(new Joystick(i));
		}
	}
	for (auto joystick : m_joysticks) {
		if (!joystick->isPresent()) {
			removeJoystick(joystick);
		}
	}
}

JoystickInput::~JoystickInput() {

}

void JoystickInput::update(double time, double deltaTime) {
    
}

/**
 * Joystick functions
 */
Joystick::Joystick(int joystickId) : 
	m_joystick_id(joystickId) 
{ 
	m_name = glfwGetJoystickName(m_joystick_id); 
}

bool Joystick::isPresent(int id) {
    return glfwJoystickPresent(id)!=0;
}


bool Joystick::isPresent() {
    return glfwJoystickPresent(m_joystick_id)!=0;
}

std::vector<float> &Joystick::getJoystickAxes() {
    m_axes.clear();
    int num_axes;
    const float *axs = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &num_axes);
    int axi = 0;

    for(int i=0; i < num_axes; ++i){
        float x = axs[i]; 
		x = threshold_abs(axs[i],0.075f);
        m_axes.push_back(x);
    }

    return m_axes;
}

std::vector<int> &Joystick::getJoystickButtons() {
    m_buttons.clear();
    int num_buttons;
    const unsigned char *buttons = glfwGetJoystickButtons(m_joystick_id, &num_buttons);
    for (int i = 0; i < num_buttons; ++i)
        m_buttons.push_back(buttons[i]);
    return m_buttons;
}

void JoystickInput::addJoystick(Joystick *joystick) {
    // Check if this joystick already exists
    for(auto j : m_joysticks)
        if(*j==*joystick) return;
    // If not, add it and notify
    m_joysticks.push_back(joystick);
    joystick_connected.notify(joystick);
}

void JoystickInput::removeJoystick(Joystick *joystick) {
    // If given joystick exists, remove it and notify
    std::vector<Joystick *>::iterator iter = std::find(m_joysticks.begin(), m_joysticks.end(), joystick);
    if (iter != m_joysticks.end()) {
        m_joysticks.erase(iter);
        joystick_disconnected.notify(joystick);
    }
}

void JoystickInput::onAttach(ecs::Entity *e) {
    m_entity = e;
}

void JoystickInput::onDetach(ecs::Entity *e) {
    m_entity = nullptr;
}
