//
// Created by Lauri Kortevaara(personal) on 20/12/15.
//

#include <exception>
#include <stdexcept>
#include <iostream>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "GL_error.h"
#include "Engine.h"
#include "system/InputSystem.h"
#include "component/KeyboardInput.h"

using namespace Omen;


std::map<GLFWwindow *, Omen::Window &> Window::window_size_changed_callbacks;
std::map<GLFWwindow *, Omen::Window &> Window::file_drop_callbacks;
Window::WindowCreated Window::signal_window_created;

Window::Window() {
    init();
}

void Window::init()
{

}

void Window::windowSizeChanged(GLFWwindow *window, int width, int height) {
    // First notify about generic key-hit event
    signal_window_size_changed.notify(width,height);
}

void Window::fileDropped(GLFWwindow *window, int count, const char** filePaths) {
    // First notify about generic key-hit event
    std::vector<std::string> vFilePaths;
    for(int i=0; i < count; ++i)
        vFilePaths.push_back(filePaths[i]);
    signal_file_dropped.notify(vFilePaths);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::createWindow(unsigned int width, unsigned int height) {
    /* Initialize the library */
    if (!glfwInit())
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + std::string(": Unable to initialize window."));


    // Enable multisampling
    glfwWindowHint(GLFW_SAMPLES, 16); // 4x antialiasing
    //glfwWindowHint(GLFW_STEREO, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

    /* Create a windowed mode window and its OpenGL context */
    m_window = glfwCreateWindow(width, height, "The Omen Game engine", NULL, NULL);
    if (m_window == nullptr) {
        glfwTerminate();
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + std::string(": Unable to create window"));
    }
    m_width = width;
    m_height = height;

    glfwSetCursorPos(m_window, m_width/2, m_height/2);
    glfwMakeContextCurrent(m_window);
    check_gl_error();

    glEnable(GL_SAMPLE_COVERAGE);

    // WindowSizeChange signal handler
    // Add a static C-function callback wrapper with pointer to this
    window_size_changed_callbacks.insert(std::pair<GLFWwindow *, Window &>(m_window, *this));
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow *win, int w, int h) -> void {
        if (window_size_changed_callbacks.find(win) != window_size_changed_callbacks.end())
            window_size_changed_callbacks.find(win)->second.windowSizeChanged(win, w, h);
    });

    // File drop callback
    file_drop_callbacks.insert(std::pair<GLFWwindow *, Window &>(m_window, *this));
    glfwSetDropCallback(m_window, [](GLFWwindow *win, int count,const char** paths) -> void {
       if(file_drop_callbacks.find(win) != file_drop_callbacks.end())
           file_drop_callbacks.find(win)->second.fileDropped(win, count, paths);
    });
    check_gl_error();

    m_swapInterval = 00; // by default 60 FPS
    glfwSwapInterval(m_swapInterval);
    check_gl_error();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    check_gl_error();
    glClearColor(0,0,0, 1.0f);
    check_gl_error();

    // Notify about window being created
    signal_window_created.notify(shared_from_this());
    check_gl_error();

    Engine* e = Engine::instance();
    KeyboardInput* ki = (KeyboardInput*)e->findComponent<KeyboardInput>();
    ki->signal_key_press.connect([&](int k, int s, int a, int m) {
        if(k == GLFW_KEY_0 || k == GLFW_KEY_F){
            m_swapInterval = m_swapInterval == 60 ? 0 : 60;
            glfwSwapInterval(m_swapInterval);
        }

    });
}

Window::~Window() {
    if(m_window != nullptr){
        glfwDestroyWindow(m_window);
    }
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

unsigned int Window::width() const {
    return size().width;
}

unsigned int Window::height() const {
    return size().height;
}

Window::_size Window::size() const {
    _size size;
    glfwGetWindowSize(m_window, &size.width, &size.height);
    return size;
}

void Window::showMouseCursor(){
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursor(m_window, glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
}

void Window::hideMouseCursor() {
    glfwSetCursor(m_window, nullptr);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::destroy() {
    glfwMakeContextCurrent(0);
    glfwDestroyWindow(m_window);
    m_window = nullptr;
}
