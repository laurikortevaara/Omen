//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_KEYBOARDINPUT_H
#define OMEN_KEYBOARDINPUT_H


#include <functional>
#include <GLFW/glfw3.h>
#include <map>
#include "../Signal.h"
#include "Component.h"


namespace Omen {
    class KeyboardInput : public Omen::ecs::Component {
        static std::map<GLFWwindow *, Omen::KeyboardInput &> keyhit_callbacks;

    public:
        typedef Omen::Signal<std::function<void(int /*key*/, int /*scanCode*/, int /*action*/, int /*mods*/)> > KeyHit_t;
        KeyHit_t signal_key_hit;
        KeyHit_t signal_key_press;
        KeyHit_t signal_key_release;
    public:
        KeyboardInput();
        virtual ~KeyboardInput();

        void keyHit(GLFWwindow *window, int key, int scanCode, int action, int mods);

        virtual void update(double time, double deltaTime);

    private:
        GLFWwindow *m_window;
    };
} // namespace Omen


#endif //OMEN_KEYBOARDINPUT_H
