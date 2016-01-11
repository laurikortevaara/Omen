//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "MouseInput.h"
#include "../Window.h"
#include "../Engine.h"
#include <glm/glm.hpp>

using namespace Omen;

std::map<GLFWwindow *, Omen::MouseInput &> MouseInput::mouseinput_callbacks;

MouseInput::MouseInput() : Component() {
    // KeyHit signal handler
    // Add a static C-function callback wrapper with pointer to this
    m_window = glfwGetCurrentContext();

    mouseinput_callbacks.insert(std::pair<GLFWwindow *, Omen::MouseInput &>(m_window, *this));

    glfwSetCursorPosCallback(m_window, [](GLFWwindow *win, double x, double y) -> void {
        if (mouseinput_callbacks.find(win) != mouseinput_callbacks.end())
            mouseinput_callbacks.find(win)->second.cursorPosChanged(win, x, y);
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow *win, int button, int action, int mods) -> void {
        if (mouseinput_callbacks.find(win) != mouseinput_callbacks.end())
            mouseinput_callbacks.find(win)->second.mouseButtonPressed(win, button, action, mods);
    });

    // Hide te cursor and disable it's movement, kinda like capturing the mouse
    //glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

MouseInput::~MouseInput() {
    if (mouseinput_callbacks.find(m_window) != mouseinput_callbacks.end())
        mouseinput_callbacks.erase(mouseinput_callbacks.find(m_window));
}

void MouseInput::cursorPosChanged(GLFWwindow *window, double x, double y) {
    m_cursorPos = {x, y};
    // First notify about generic key-hit event
    if (m_isEnabled)
        signal_cursorpos_changed.notify(x, y);
}

void MouseInput::update(double time, double deltaTime) {
}

void MouseInput::mouseButtonPressed(GLFWwindow *window, int button, int action, int mods) {
    if (m_isEnabled)
        signal_mousebutton_pressed.notify(button, action, mods);
}
