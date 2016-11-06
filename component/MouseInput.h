//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_MOUSEINPUT_H
#define OMEN_MOUSEINPUT_H


#include <functional>
#include <map>
#include "../Signal.h"
#include "Component.h"
#include <glm/vec2.hpp>
#include "../Engine.h"

struct GLFWwindow;

namespace omen {
    class MouseInput : public omen::ecs::Component {
        static std::map<GLFWwindow *, omen::MouseInput &> mouseinput_callbacks;
    protected:
        virtual void onAttach(ecs::Entity* e);
        virtual void onDetach(ecs::Entity* e);
    public:
        typedef omen::Signal<std::function<void(omen::floatprec, omen::floatprec)> > CursorPos_t;
        typedef omen::Signal<std::function<void(int, int, int)> > ButtonPress_t;
        CursorPos_t signal_cursorpos_changed;
        ButtonPress_t signal_mousebutton_pressed;
    public:
        MouseInput();
        virtual ~MouseInput();

        void cursorPosChanged(GLFWwindow *window, omen::floatprec x, omen::floatprec y);
        void mouseButtonPressed(GLFWwindow *window, int button, int action, int mods );

        virtual void update(double time, double deltaTime);

        glm::vec2 cursorPos() const {return m_cursorPos;}

    private:
        GLFWwindow *m_window;
        glm::vec2  m_cursorPos;
    };
} // namespace omen


#endif //OMEN_MOUSEINPUT_H
