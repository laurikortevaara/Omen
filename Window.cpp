//
// Created by Lauri Kortevaara on 20/12/15.
//

#include "Omen_OpenGL.h"

#include <memory>
#include <exception>
#include <stdexcept>
#include <iostream>
#include "Window.h"
#include "GL_error.h"
#include "Engine.h"
#include "system/InputSystem.h"
#include "component/KeyboardInput.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
using namespace omen;


std::map<GLFWwindow *, omen::Window &> Window::window_size_changed_callbacks;
std::map<GLFWwindow *, omen::Window &> Window::file_drop_callbacks;
Window::WindowCreated Window::signal_window_created;

Window::Window() :m_fullscreen(false) {
	init();
}

void Window::init()
{

}

void Window::windowSizeChanged(GLFWwindow *window, int width, int height) {
	// notify about window size changed event
	m_width = width;
	m_height = height;
	signal_window_size_changed.notify(width, height);
}

void Window::fileDropped(GLFWwindow *window, int count, const char** filePaths) {
	// notify about file drop event
	std::vector<std::string> vFilePaths;
	for (int i = 0; i < count; ++i)
		vFilePaths.push_back(filePaths[i]);
	signal_file_dropped.notify(vFilePaths);
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(m_window) == GL_TRUE;
}

// Add new popup menu
#define ADDPOPUPMENU(hmenu, string) \
	HMENU hSubMenu = CreatePopupMenu(); \
	AppendMenu(hmenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, string);


// Add a menu item
#define ADDMENUITEM(hmenu, ID, string) \
	AppendMenu(hSubMenu, MF_STRING, ID, string);


enum
{
	ID_FILE_EXIT,
	ID_FILE_MSG
}; 

void CreateMainMenu(HWND hWnd)
{
	HMENU hMenu = CreateMenu();
	ADDPOPUPMENU(hMenu, "&File");
	ADDMENUITEM(hMenu, ID_FILE_EXIT, "&Exit");
	ADDMENUITEM(hMenu, ID_FILE_MSG, "&Show message");
	SetMenu(hWnd, hMenu);
}

void Window::createWindow(int width, int height) {
	int minor, major, rev;
	glfwGetVersion(&major, &minor, &rev);
	const char* versionstr = glfwGetVersionString();
	//setenv("DISPLAY", ":0.0", 1);
	glfwSetErrorCallback([](int errorcode, const char* errorstring) {
		std::cerr << "Error in glfw: " << errorcode << ", " << errorstring << std::endl;
	});

	if (!glfwInit())
		return;
	//throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + std::string(": Unable to initialize window."));

	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.1
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

		/* Create a windowed mode window and its OpenGL context */
	m_window = glfwCreateWindow(width, height, "The omen Game engine", m_fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (m_window == nullptr) {
		glfwTerminate();
#ifdef __APPLE__
		throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + std::string(": Unable to create window"));
#elif WIN32
		throw std::runtime_error(std::string(__FUNCTION__) + std::string(": Unable to create window"));
#endif
	}
	glfwGetWindowSize(m_window, &width, &height);
	m_width = width;
	m_height = height;

	// Create the main menu
	CreateMainMenu(glfwGetWin32Window(m_window));

	//glfwSetCursorPos(m_window, m_width/2, m_height/2);
	glfwMakeContextCurrent(m_window);
	glfwShowWindow(m_window);
	check_gl_error();
	
	glEnable(GL_SAMPLE_COVERAGE);
	int w = width;
	int h = height;
	//glfwGetFramebufferSize(m_window, &w, &h);
	Engine::instance()->setViewport(0, 0, w, h);

	/* Initialize the library */
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("glewInit failed");


	// WindowSizeChange signal handler
	// Add a static C-function callback wrapper with pointer to this
	window_size_changed_callbacks.insert(std::pair<GLFWwindow *, Window &>(m_window, *this));
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow *win, int w, int h) -> void {
		if (window_size_changed_callbacks.find(win) != window_size_changed_callbacks.end())
			window_size_changed_callbacks.find(win)->second.windowSizeChanged(win, w, h);
	});

	// File drop callback
	file_drop_callbacks.insert(std::pair<GLFWwindow *, Window &>(m_window, *this));
	glfwSetDropCallback(m_window, [](GLFWwindow *win, int count, const char** paths) -> void {
		if (file_drop_callbacks.find(win) != file_drop_callbacks.end())
			file_drop_callbacks.find(win)->second.fileDropped(win, count, paths);
	});

	check_gl_error();

	m_swapInterval = 0; // by default 60 FPS
	glfwSwapInterval(m_swapInterval);
	check_gl_error();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	check_gl_error();
	glClearColor(0, 0, 0, 1.0f);
	check_gl_error();

	//glfwSetCursor(m_window, nullptr);
	check_gl_error();
	// Notify about window being created
	signal_window_created.notify(this);
	check_gl_error();

	Engine* e = Engine::instance();
	KeyboardInput* ki = (KeyboardInput*)e->findComponent<KeyboardInput>();
	/*ki->signal_key_press.connect(this,[&](int k, int s, int a, int m) {
		if (k == GLFW_KEY_0 || k == GLFW_KEY_F) {
			m_swapInterval = m_swapInterval == 60 ? 0 : 60;
			glfwSwapInterval(m_swapInterval);
		}

	});*/

	hideMouseCursor();
}

Window::~Window() {
	if (m_window != nullptr) {
		glfwDestroyWindow(m_window);
	}
}

void Window::start_rendering() {
	glfwMakeContextCurrent(m_window);
	glEnable(GL_SCISSOR_TEST);
	// Set the clear color
	//easeOutElastic: function (t) { return .04 * t / (--t) * Math.sin(25 * t) },
	double t = Engine::instance()->time();
	double w = 500+5000*(0.04*t / (--t)*sin(25 * t));

	glViewport(0, 0, m_width, m_height);
	glScissor(0, 0, m_height, m_height);
	glClearColor(0, 0, 0, 1);
	
	/*glFlush();
	glViewport(0, 100, m_width, m_height - 100);
	glScissor(0, 100, m_width, m_height - 100);
	glClearColor(1, 0, 0, 1);
	*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();

}

void Window::end_rendering() {
	/* Swap front and back buffers */
	glfwSwapBuffers(m_window);

	/* Poll for and process events */
	glfwPollEvents();
}

unsigned int Window::width() const {
	return size().width;
}

unsigned int Window::height() const {
	return size().height;
}

unsigned int Window::frameBufferWidth() const {
	return size().fb_width;
}

unsigned int Window::frameBufferHeight() const {
	return size().fb_height;
}

Window::_size Window::size() const {
	_size size;
	glfwGetWindowSize(m_window, &size.width, &size.height);
	glfwGetFramebufferSize(m_window, &size.fb_width, &size.fb_height);
	return size;
}

void Window::showMouseCursor() {
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
