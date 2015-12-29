//
// Created by Lauri Kortevaara(personal) on 20/12/15.
//

#include <exception>
#include <stdexcept>
#include <iostream>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include "Window.h"

using namespace Omen;


std::map<GLFWwindow *, Omen::Window &> Window::window_size_changed_callbacks;
Window::WindowCreated Window::signal_window_created;

Window::Window(unsigned int width, unsigned int height) {
    init();
}

void Window::windowSizeChanged(GLFWwindow *window, int width, int height) {
    // First notify about generic key-hit event
    signal_window_size_changed.notify(width,height);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::init() {
    /* Initialize the library */
    if (!glfwInit())
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + std::string(": Unable to initialize window."));

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

    /* Create a windowed mode window and its OpenGL context */
    m_window = glfwCreateWindow(1280, 720, "The Omen Game engine", NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + std::string(": Unable to create window"));
    }

    // WindowSizeChange signal handler
    // Add a static C-function callback wrapper with pointer to this
    window_size_changed_callbacks.insert(std::pair<GLFWwindow *, Window &>(m_window, *this));
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow *win, int w, int h) -> void {
        if (window_size_changed_callbacks.find(win) != window_size_changed_callbacks.end())
            window_size_changed_callbacks.find(win)->second.windowSizeChanged(win, w, h);
    });
    // Enable multisampling
    glfwWindowHint(GLFW_SAMPLES, 16);
    glEnable(GL_MULTISAMPLE);

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClearColor(0,0,0, 1.0f);

    glfwSetCursor(m_window, nullptr);
    // Notify about window being created
    signal_window_created.notify(this);
}

Window::~Window() {
}

void Window::start_rendering() {
    // Set the clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::end_rendering() {
    /* Swap front and back buffers */
    glfwSwapBuffers(m_window);

    /* Poll for and process events */
    glfwPollEvents();
}

bool Window::keyPressed(unsigned int key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}
