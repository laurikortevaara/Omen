//
// Created by Lauri Kortevaara(personal) on 20/12/15.
//


#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <sstream>
#include <iomanip>
#include <queue>

#include "Engine.h"
#include "GL_error.h"
#include "system/InputSystem.h"
#include "component/KeyboardInput.h"
#include "component/MouseInput.h"
#include "utils.h"


using namespace Omen;

// Singleton instance
Engine *Engine::m_instance = nullptr;

Engine *Engine::instance() {
    if (m_instance == nullptr)
        m_instance = new Engine();
    return m_instance;
}


void Engine::initializeSystems() {
    // Initialize input systems
    InputSystem *inputSystem = new InputSystem();

    // Keyboard input
    KeyboardInput *keyboardInput = new KeyboardInput();
    inputSystem->add(keyboardInput);

    // Mouse input
    MouseInput *mouseInput = new MouseInput();
    inputSystem->add(mouseInput);

    // Joystick input
    JoystickInput *joystickInput = new JoystickInput();
    inputSystem->add(joystickInput);
    m_systems.push_back(inputSystem);

    joystickInput->joystick_connected.connect([&](Joystick *joystick) {
        m_joystick = joystick;
    });
    joystickInput->joystick_disconnected.connect([&](Joystick *joystick) {
        m_joystick = nullptr;
    });


    // Connect key-hit, -press and -release signals to observers
    keyboardInput->signal_key_hit.connect([this](int k, int s, int a, int m) {
        keyHit(k, s, a, m);
    });

    keyboardInput->signal_key_press.connect([](int k, int s, int a, int m) {
    });

    keyboardInput->signal_key_release.connect([](int k, int s, int a, int m) {
    });

    // Connect key-hit, -press and -release signals to observers
    m_window->signal_window_size_changed.connect([this](int width, int height) {
        m_camera->onWindowSizeChanged(width, height);
    });
}

Engine::Engine() :
        m_scene(nullptr),
        m_camera(nullptr),
        m_window(nullptr),
        m_joystick(nullptr),
        m_time(0),
        m_timeDelta(0),
        m_framecounter(0) {

    std::string currentDir = Omen::getWorkingDir();
    if (currentDir.find("bin") == std::string::npos)
        chdir("bin");

    Window::signal_window_created.connect([this](std::shared_ptr<Window> window) {
        m_window = window;

        initializeSystems();

        m_camera = new Camera("Camera1", {0, 0, 0}, {0, 0, 0.01f}, 60.0f);
        m_scene = std::make_unique<Scene>();
        m_text = std::make_unique<TextRenderer>();
    });
}

void Engine::update() {
    m_timeDelta = glfwGetTime() - m_time;
    m_time = glfwGetTime();
    signal_engine_update.notify(m_time, m_timeDelta);
}

double Engine::time() {
    return glfwGetTime();
}

void Engine::renderScene() {
    if (m_scene != nullptr)
        m_scene->render(m_camera->mvp(), m_camera->viewMatrix());
}

void Engine::render() {
    m_framecounter++;
    m_window->start_rendering();
    check_gl_error();
    renderScene();
    check_gl_error();

    //
    // Render FPS counter as text
    //
    Window::_size s = m_window->size();
    float sx = (float) (2.0 / s.width);
    float sy = (float) (2.0 / s.height);
    static std::vector<double> q_fps;
    if (q_fps.size() < 500)
        q_fps.push_back(1.0 / m_timeDelta);
    else {
        q_fps.erase(q_fps.begin());
        q_fps.push_back(1.0 / m_timeDelta);
    }

    double avg_fps = 0.0;
    for (auto fps : q_fps)
        avg_fps += fps;
    avg_fps /= q_fps.size();

    std::ostringstream os;
    std::vector<float> axes = m_joystick != nullptr ? m_joystick->getJoystickAxes() : std::vector<float>({0, 0, 0, 0});
    os << "FPS: " << std::setprecision(3) << avg_fps << " [" << std::setprecision(2) << (1.0 / avg_fps) * 1000.0 <<
    " ms./frame]" << "\nFRAME:(" << m_framecounter << ")\nMEM:xxx MB"\
 << "\nJOYSTICK:[" << axes[0] << ", " << axes[1] << ", " << axes[2] << ", " << axes[3] << "]";
    std::string text(os.str());
    m_text->render_text(text.c_str(), 14.0, -1 + 8 * sx, 1 - 14 * sy, sx, sy, glm::vec4(1, 1, 1, 1));

    m_window->end_rendering();
}

std::shared_ptr<Window> Engine::createWindow(unsigned int width, unsigned int height) {
    m_window = std::make_shared<Window>();
    m_window->createWindow(width, height);
    check_gl_error();
    glFrontFace(GL_CCW);
    check_gl_error();
    glDisable(GL_CULL_FACE);
    check_gl_error();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    return m_window;
}

void Engine::keyHit(int key, int scanCode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
            exit(0);

        /*if (m_scene != nullptr) {
            if (key == GLFW_KEY_W) {
                for (auto model : m_scene->m_models)
                    if (model->m_mesh->fInnerTess > 1)
                        model->m_mesh->fInnerTess -= 1;
            }
            if (key == GLFW_KEY_E) {
                for (auto model : m_scene->m_models)
                    model->m_mesh->fInnerTess += 1;
            }
            if (key == GLFW_KEY_S) {
                for (auto model : m_scene->m_models)
                    if (model->m_mesh->fOuterTess > 1)
                        model->m_mesh->fOuterTess -= 1;
            }
            if (key == GLFW_KEY_D) {
                for (auto model : m_scene->m_models)
                    model->m_mesh->fOuterTess += 1;
            }

            if (key == GLFW_KEY_T)
                for (auto model : m_scene->m_models)
                    model->m_mesh->mPolygonMode = GL_FILL;
            if (key == GLFW_KEY_Y)
                for (auto model : m_scene->m_models)
                    model->m_mesh->mPolygonMode = GL_LINE;

            if (key == GLFW_KEY_1)
                for (auto model : m_scene->m_models)
                    model->m_mesh->m_use_texture = (++model->m_mesh->m_use_texture) % 3;
        }*/
    }

}