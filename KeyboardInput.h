//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_KEYBOARDINPUT_H
#define OMEN_KEYBOARDINPUT_H


#include <functional>
#include <GLFW/glfw3.h>
#include "Component.h"
#include "Signal.h"


namespace Omen {
    class KeyboardInput : public Omen::Component {
        static std::map<GLFWwindow *, Omen::KeyboardInput &> keyhit_callbacks;

    public:
        typedef Omen::Signal<std::function<void(int /*key*/, int /*scanCode*/, int /*action*/, int /*mods*/)> > KeyHit;
        KeyHit signal_key_hit;
        KeyHit signal_key_press;
        KeyHit signal_key_release;
    public:
        KeyboardInput();
        virtual ~KeyboardInput();

        void keyHit(GLFWwindow *window, int key, int scanCode, int action, int mods);

    private:
        GLFWwindow *m_window;
    };
} // namespace Omen


#endif //OMEN_KEYBOARDINPUT_H
