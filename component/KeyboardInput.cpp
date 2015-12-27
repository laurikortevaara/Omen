//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "KeyboardInput.h"
#include "../Window.h"
#include "../Engine.h"

using namespace Omen;

std::map<GLFWwindow *, Omen::KeyboardInput &> KeyboardInput::keyhit_callbacks;

KeyboardInput::KeyboardInput() : Component() {
    // KeyHit signal handler
    // Add a static C-function callback wrapper with pointer to this
    m_window = glfwGetCurrentContext();

    keyhit_callbacks.insert(std::pair<GLFWwindow *, Omen::KeyboardInput &>(m_window, *this));
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

    // First notify about specializaed key-press and -relese events
    switch (action) {
        case GLFW_PRESS:
            signal_key_press.notify(key, scanCode, action, mods);
            break;
        case GLFW_RELEASE:
            signal_key_release.notify(key, scanCode, action, mods);
            break;
        default:
            break;
    }
}

void KeyboardInput::update(double time, double deltaTime) {
}
