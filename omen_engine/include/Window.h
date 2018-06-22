//
// Created by Lauri Kortevaara on 20/12/15.
//

#ifndef OMEN_WINDOW_H
#define OMEN_WINDOW_H

#include "Signal.h"

#include <memory>
#include <functional>

struct GLFWwindow;

namespace omen {

    // Key type wrapper
    #define OMEN_KEY(key) {GLFW_KEY_#key}

    class Window : public std::enable_shared_from_this<Window> {
    public:
        Window();

        typedef Signal<std::function<void (int width, int height)> > WindowSizeChanged;
        WindowSizeChanged signal_window_size_changed;

        typedef Signal<std::function<void (Window*) > > WindowCreated;
        static WindowCreated signal_window_created;

        typedef Signal<std::function<void (const std::vector<std::string>& filePaths) > > FileDropped;
        FileDropped signal_file_dropped;

    private:
        static std::map<GLFWwindow*,omen::Window&>      window_size_changed_callbacks;
        static std::map<GLFWwindow*,omen::Window&>      file_drop_callbacks;

    public:
        ~Window();
        void createWindow(int width, int height);
        void destroy();

        bool shouldClose() const;
        void start_rendering();
        void end_rendering();

        void showMouseCursor();
        void hideMouseCursor();

        unsigned int width() const;
        unsigned int height() const;
        unsigned int frameBufferWidth() const;
        unsigned int frameBufferHeight() const;
        struct _size{
            int width, height;
            int fb_width, fb_height; // frame buffer width,height
        };
        _size size() const;

		float aspectRatio() const;

		GLFWwindow* window() const { return m_window; }

    private:
        void init();

        void keyHit(GLFWwindow *window, int key, int scanCode, int action, int mods);
        void windowSizeChanged(GLFWwindow *window, int width, int height);
        GLFWwindow* m_window;
        unsigned int m_width;
        unsigned int m_height;
        int m_swapInterval;

        void fileDropped(GLFWwindow *window, int count, const char **filePaths);


        bool m_fullscreen;


    };

} // namespace omen


#endif //OMEN_WINDOW_H
