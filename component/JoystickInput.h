//
// Created by Lauri Kortevaara(personal) on 27/12/15.
//

#ifndef OMEN_JOYSTICKINPUT_H
#define OMEN_JOYSTICKINPUT_H


#include <functional>
#include <map>
#include <string>
#include "../Signal.h"
#include "Component.h"

struct GLFWwindow;

namespace omen {
    class Joystick {
        int m_joystick_id; // from 0-15 as in GLFW3
        std::vector<float>  m_axes;
        std::vector<int>    m_buttons;
        std::string         m_name;
    public:
        static const int MAX_JOYSTICK_COUNT = 16;
        static const int X_AXIS = 0;
        static const int Y_AXIS = 3;
        static const int Z_AXIS = 1;

        enum JOYSTICK_BUTTON{
            SELECT = 0,
            LEFT_ANALOG = 1,
            RIGHT_ANALOG = 2,
            START = 3,
            UP = 4,
            RIGHT = 5,
            DOWN = 6,
            LEFT = 7,
            L2 = 8,
            R2 = 9,
            L1 = 10,
            R1 = 11,
            TRIANGLE = 12,
            CIRCLE = 13,
            X = 14,
            SQUARE = 15,
            PS = 16
        };

        Joystick(int joystickId);

        static bool isPresent(int id);
        bool isPresent();

        bool operator==(const Joystick& other) {return m_joystick_id==other.m_joystick_id;}

        std::vector<float>& getJoystickAxes();
        std::vector<int>& getJoystickButtons();

    };

    class JoystickInput : public omen::ecs::Component {
    protected:
        virtual void onAttach(ecs::Entity* e);
        virtual void onDetach(ecs::Entity* e);
    public:
        typedef omen::Signal<std::function<void(Joystick*)> > JoystickInput_t;
        JoystickInput_t joystick_connected;
        JoystickInput_t joystick_disconnected;
        JoystickInput_t joystick_button_pressed;
        JoystickInput_t joystick_button_released;
        JoystickInput_t joystick_generic_input;
        JoystickInput_t joystick_axis_moved;
    public:
        JoystickInput();
        virtual ~JoystickInput();

        virtual void update(double time, double deltaTime);

    private:
        GLFWwindow *m_window;

        void addJoystick(Joystick *joystick);
        void removeJoystick(Joystick *joystick);
        std::vector<Joystick*> m_joysticks;
    };
} // namespace omen


#endif //OMEN_JOYSTICKINPUT_H
