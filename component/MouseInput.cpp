//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "MouseInput.h"
#include "../Window.h"
#include "../Engine.h"
#include <glm/glm.hpp>

using namespace omen;

std::map<GLFWwindow *, omen::MouseInput &> MouseInput::mouseinput_callbacks;

MouseInput::CursorPos_t MouseInput::signal_cursorpos_changed;

MouseInput::MouseInput() : Component() {
    // KeyHit signal handler
    // Add a static C-function callback wrapper with pointer to this
    m_window = glfwGetCurrentContext();

    mouseinput_callbacks.insert(std::pair<GLFWwindow *, omen::MouseInput &>(m_window, *this));

    glfwSetCursorPosCallback(m_window, [](GLFWwindow *win, double x, double y) -> void {
        if (mouseinput_callbacks.find(win) != mouseinput_callbacks.end())
            mouseinput_callbacks.find(win)->second.cursorPosChanged(win, static_cast<omen::floatprec>(x), static_cast<omen::floatprec>(y));
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow *win, int button, int action, int mods) -> void {
        if (mouseinput_callbacks.find(win) != mouseinput_callbacks.end())
            if(action==GLFW_PRESS)
				mouseinput_callbacks.find(win)->second.mouseButtonPressed(win, button, action, mods);
			if(action==GLFW_RELEASE)
				mouseinput_callbacks.find(win)->second.mouseButtonReleased(win, button, action, mods);
    });

	glfwSetScrollCallback(m_window, [](GLFWwindow* win, double x, double y) -> void {
		if (mouseinput_callbacks.find(win) != mouseinput_callbacks.end())
			mouseinput_callbacks.find(win)->second.mouseScrolled(win, x, y);
	});

    // Hide te cursor and disable it's movement, kinda like capturing the mouse
    //glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

MouseInput::~MouseInput() {
    if (mouseinput_callbacks.find(m_window) != mouseinput_callbacks.end())
        mouseinput_callbacks.erase(mouseinput_callbacks.find(m_window));
}

void MouseInput::cursorPosChanged(GLFWwindow *window, omen::floatprec x, omen::floatprec y) {
    m_cursorPos = {x, y};
    // First notify about generic key-hit event
    if (m_isEnabled)
        signal_cursorpos_changed.notify(x, y);
}

void MouseInput::updateMouseButtonStates() {
	m_buttonLeftPressed = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) & GLFW_PRESS;
	m_buttonRightPressed = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) & GLFW_PRESS;
	m_buttonLeftPressed = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) & GLFW_PRESS;
}

void MouseInput::update(double time, double deltaTime) {
	updateMouseButtonStates();
}

void MouseInput::mouseButtonPressed(GLFWwindow *window, int button, int action, int mods) {
	updateMouseButtonStates();
	if (m_isEnabled)
		signal_mousebutton_pressed.notify(button, action, mods);

}

void MouseInput::mouseButtonReleased(GLFWwindow *window, int button, int action, int mods) {
	updateMouseButtonStates();
	if (m_isEnabled)
		signal_mousebutton_released.notify(button, action, mods);
}

void MouseInput::mouseScrolled(GLFWwindow *window, omen::floatprec x, omen::floatprec y) {
	if (m_isEnabled)
		signal_mouse_scrolled.notify(x, y);
}

void MouseInput::onAttach(ecs::Entity *e) {
    m_entity = e;
}

void MouseInput::onDetach(ecs::Entity *e) {
    m_entity = nullptr;
}
