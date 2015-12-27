//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_MOUSEINPUT_H
#define OMEN_MOUSEINPUT_H


#include <functional>
#include <GLFW/glfw3.h>
#include <map>
#include "../Signal.h"
#include "Component.h"


namespace Omen {
    class MouseInput : public Omen::ecs::Component {
        static std::map<GLFWwindow *, Omen::MouseInput &> cursorpos_callbacks;

    public:
        typedef Omen::Signal<std::function<void(double, double)> > CursorPos_t;
        CursorPos_t signal_cursorpos_changed;
    public:
        MouseInput();
        virtual ~MouseInput();

        void cursorPosChanged(GLFWwindow *window, double x, double y);

        virtual void update(double time, double deltaTime);

    private:
        GLFWwindow *m_window;
    };
} // namespace Omen


#endif //OMEN_MOUSEINPUT_H
