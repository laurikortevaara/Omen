//
// Created by Lauri Kortevaara(personal) on 20/12/15.
//

#ifndef OMEN_WINDOW_H
#define OMEN_WINDOW_H

#include <GLFW/glfw3.h>
#include "Signal.h"

namespace Omen {

    // Key type wrapper
    #define OMEN_KEY(key) {GLFW_KEY_#key}

    class Window {
    public:
        typedef Signal< std::function<void (int width, int height)> > WindowSizeChanged;
        WindowSizeChanged signal_window_size_changed;

        typedef Signal< std::function<void (Window*) > > WindowCreated;
        static WindowCreated signal_window_created;

    private:
        static std::map<GLFWwindow*,Omen::Window&> window_size_changed_callbacks;

    public:
        Window(unsigned int width, unsigned int height);
        ~Window();

        bool keyPressed(unsigned int key) const;

        bool shouldClose() const;
        void start_rendering();
        void end_rendering();

    private:
        void init();

        void keyHit(GLFWwindow *window, int key, int scanCode, int action, int mods);
        void windowSizeChanged(GLFWwindow *window, int width, int height);
        GLFWwindow* m_window;
    };

} // namespace Omen


#endif //OMEN_WINDOW_H