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


namespace omen {
    class KeyboardInput : public omen::ecs::Component {
        static std::map<GLFWwindow *, omen::KeyboardInput &> keyhit_callbacks;
    protected:
        virtual void onAttach(ecs::Entity* e);
        virtual void onDetach(ecs::Entity* e);
    public:
        typedef omen::Signal<std::function<void(int /*key*/, int /*scanCode*/, int /*action*/, int /*mods*/)> > KeyHit_t;
        KeyHit_t signal_key_hit;
        KeyHit_t signal_key_press;
        KeyHit_t signal_key_release;
    public:
        KeyboardInput();
        virtual ~KeyboardInput();

        void keyHit(GLFWwindow *window, int key, int scanCode, int action, int mods);

        virtual void update(double time, double deltaTime);

        bool keyPressed(unsigned int key) const;
        bool keyModifierPressed(unsigned int mod) const;

    private:
        GLFWwindow *m_window;
        unsigned short m_mods;
    };
} // namespace omen


#endif //OMEN_KEYBOARDINPUT_H
