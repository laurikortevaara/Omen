//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "component/KeyboardInput.h"

#include "Window.h"
#include "Engine.h"

#include <memory>

using namespace omen;

std::map<GLFWwindow *, omen::KeyboardInput &> KeyboardInput::keyhit_callbacks;

KeyboardInput::KeyboardInput() : Component() {
    // KeyHit signal handler
    // Add a static C-function callback wrapper with pointer to this
    m_window = glfwGetCurrentContext();

    keyhit_callbacks.insert(std::pair<GLFWwindow *, omen::KeyboardInput &>(m_window, *this));
    glfwSetKeyCallback(m_window, [](GLFWwindow *win, int k, int s, int a, int m) -> void {
        if (keyhit_callbacks.find(win) != keyhit_callbacks.end())
            keyhit_callbacks.find(win)->second.keyHit(win, k, s, a, m);
    });

}

KeyboardInput::~KeyboardInput() {
    if (keyhit_callbacks.find(m_window) != keyhit_callbacks.end())
        keyhit_callbacks.erase(keyhit_callbacks.find(m_window));
}

void KeyboardInput::keyHit(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    // First notify about generic key-hit event
    signal_key_hit.notify(key, scanCode, action, mods);

    m_mods = mods;

    // First notify about specializaed key-press and -relese events
    switch (action) {
        case GLFW_PRESS:
			std::cout << "Press" << std::endl;
            signal_key_press.notify(key, scanCode, action, mods);
            break;
        case GLFW_RELEASE:
            signal_key_release.notify(key, scanCode, action, mods);
            break;
        default:
            break;
    }
}

bool KeyboardInput::keyPressed(unsigned int key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

/**
 * GLFW_MOD_SHIFT
 * GLFW_MOD_ALT
 * GLFW_MOD_CTRL
 */
bool KeyboardInput::keyModifierPressed(unsigned int mod) const {
    return (m_mods & (mod>0));
}

void KeyboardInput::update(double time, double deltaTime) {
}

void KeyboardInput::onAttach(ecs::Entity *e) {
    m_entity = e;
}

void KeyboardInput::onDetach(ecs::Entity *e) {
    m_entity = nullptr;
}
