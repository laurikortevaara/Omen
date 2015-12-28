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
    MouseInput* mouseInput = new MouseInput();
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
        m_time(0),
        m_timeDelta(0),
        m_framecounter(0){
    Window::signal_window_created.connect([this](Window *window) {
        m_window = window;

        initializeSystems();

        m_camera = new Camera("Camera1", {0, 0, 0}, {0, 0, 0.01f}, 60.0f);
        m_scene = new Scene();
        m_shader = new Shader("shaders/pass_through.glsl");
        m_texture = new Texture("checker.jpg");
        m_texture2 = new Texture("test.jpg");
        m_text = new TextRenderer();


        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        check_gl_error();

        /**
         * Setup the vertex coordinate buffer object (vbo)
         */
        GLfloat s = 1000;
        // Enable vertex attributes
        m_vcoord_attrib = m_shader->getAttribLocation("position");
        if (m_vcoord_attrib >= 0) {
            glEnableVertexAttribArray(m_vcoord_attrib);

            GLfloat vertices[4][3] = {
                    {-s, -.8, -s},
                    {s,  -.8, -s},
                    {s,  -.8, s},
                    {-s, -.8, s}};

            int i = sizeof(vertices);
            // Create vbo
            glGenBuffers(1, &m_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(m_vcoord_attrib, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                                  0/*stride*/, 0/*offset*/);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            check_gl_error();

            /**
             * Setup the vertex index element buffer (ibo)
             */
            GLuint indices[6] = {0, 1, 3, 1, 2, 3};
            glGenBuffers(1, &m_ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            check_gl_error();
        }

        /**
         * Setup the vertex texture coordinates
         */
        m_tcoord_attrib = m_shader->getAttribLocation("texcoord");
        check_gl_error();
        if (m_tcoord_attrib >= 0) {
            check_gl_error();
            GLfloat texcoords[4][2] = {
                    {0, 0},
                    {s, 0},
                    {s, s},
                    {0, s}
            };
            glGenBuffers(1, &m_vbo_texcoord);
            check_gl_error();
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoord);
            check_gl_error();
            glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
            check_gl_error();
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            check_gl_error();
        }
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
    m_shader->use();
    glm::mat4x4 mvp = m_camera->mvp();
    m_shader->setUniformMatrix4fv("ModelViewProjection", 1, &mvp[0][0], false);
    m_shader->setUniform1f("Time", (float) time());

    // Set the texture map
    GLuint iTexture = 0;
    glActiveTexture(GL_TEXTURE0 + iTexture);
    m_texture->bind();
    m_shader->setUniform1i("Texture", iTexture);

    iTexture = 1;
    glActiveTexture(GL_TEXTURE0 + iTexture);
    m_texture2->bind();
    m_shader->setUniform1i("Texture2", iTexture);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(m_vcoord_attrib);
    glVertexAttribPointer(m_vcoord_attrib, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/, 0/*stride*/,
                          0/*offset*/);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoord);
    glEnableVertexAttribArray(m_tcoord_attrib);
    glVertexAttribPointer(m_tcoord_attrib, 2/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/, 0/*stride*/,
                          0/*offset*/);

    //glDrawArrays(GL_QUADS, 0, 4);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) nullptr);

    if(m_scene!= nullptr)
        m_scene->render(m_camera->mvp());
}

void Engine::render() {
    m_framecounter++;
    m_window->start_rendering();

    renderScene();

    glBindVertexArray(m_vao);
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
    std::vector<float> axes = m_joystick!= nullptr?m_joystick->getJoystickAxes() : std::vector<float>({0,0,0,0});
    os << "FPS: " << std::setprecision(3) << avg_fps << "\nFRAME:(" << m_framecounter << ")\nMEM:12MB"\
    << "\nJOYSTICK:[" << axes[0] << ", " << axes[1] << ", " << axes[2] << ", " << axes[3] << "]";
    std::string text(os.str());
    m_text->render_text(text.c_str(), 14.0, -1 + 8 * sx, 1 - 14 * sy, sx, sy, glm::vec4(1, 1, 1, 1));

    m_window->end_rendering();
    glBindVertexArray(0);
}

Window *Engine::createWindow(unsigned int width, unsigned int height) {
    m_window = new Window(width, height);

    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);

    return m_window;
}

void Engine::keyHit(int key, int scanCode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
            exit(0);

        if (m_scene != nullptr) {
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
        }
    }

}