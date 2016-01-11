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
#include <glm/vec2.hpp>


namespace Omen {
    class MouseInput : public Omen::ecs::Component {
        static std::map<GLFWwindow *, Omen::MouseInput &> mouseinput_callbacks;

    public:
        typedef Omen::Signal<std::function<void(double, double)> > CursorPos_t;
        typedef Omen::Signal<std::function<void(int, int, int)> > ButtonPress_t;
        CursorPos_t signal_cursorpos_changed;
        ButtonPress_t signal_mousebutton_pressed;
    public:
        MouseInput();
        virtual ~MouseInput();

        void cursorPosChanged(GLFWwindow *window, double x, double y);
        void mouseButtonPressed(GLFWwindow *window, int button, int action, int mods );

        virtual void update(double time, double deltaTime);

        glm::vec2 cursorPos() const {return m_cursorPos;}

    private:
        GLFWwindow *m_window;
        glm::vec2  m_cursorPos;
    };
} // namespace Omen


#endif //OMEN_MOUSEINPUT_H
