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
#include "thirdparty/glfw/include/GLFW/glfw3.h"
#include "MD3Loader.h"
#include "Picker.h"
#include "system/CoreSystem.h"
#include "component/CameraController.h"


using namespace Omen;

// Singleton instance
Engine *Engine::m_instance = nullptr;

Engine::Engine() :
        m_scene(nullptr),
        m_camera(nullptr),
        m_window(nullptr),
        m_joystick(nullptr),
        m_time(0),
        m_timeDelta(0),
        m_framecounter(0),
        m_sample_coverage(1),
        m_currentSelection(nullptr) {

    std::string currentDir = Omen::getWorkingDir();
    if (currentDir.find("bin") == std::string::npos)
        chdir("bin");

    Window::signal_window_created.connect([this](std::shared_ptr<Window> window) {
        m_window = window;

        initializeSystems();

        m_camera = new Camera("Camera1", {0, 1, -2}, {0, 0, 0}, 60.0f);
        findComponent<CameraController>()->setCamera(m_camera);qq =87
        m_scene = std::make_unique<Scene>();
        m_text = std::make_unique<TextRenderer>();

        createFramebuffer();

        Omen::MD3Loader loader;
        loader.loadModel("models/sphere.md3");
        std::vector<std::shared_ptr<Omen::Mesh>> meshes;
        for(int i=0; i < 10; ++i )
        {
            loader.getMesh(meshes);
            m_currentSelection = nullptr;
            std::shared_ptr<Model> model = std::make_shared<Model>(meshes.front());
            m_currentSelection = model.get();
            model->m_mesh->m_amplitude = 0.0;
            model->m_mesh->m_transform.pos() = {Omen::random(-10, 10), Omen::random(1, 5), Omen::random(-10, 10)};
            model->m_mesh->m_transform.scale({Omen::random(1, 4), Omen::random(1, 4), Omen::random(1, 4)});
            m_scene->m_models.push_back(model);
            meshes.clear();
        }

        initPhysics();
    });
}

Engine *Engine::instance() {
    if (m_instance == nullptr)
        m_instance = new Engine();
    return m_instance;
}

void Engine::initPhysics() {
    m_broadphase = new btDbvtBroadphase();
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_solver = new btSequentialImpulseConstraintSolver;
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0, -0.981, 0));

    m_groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
    m_groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, m_groundMotionState, m_groundShape, btVector3(0, 0, 0));
    m_groundRigidBody = new btRigidBody(groundRigidBodyCI);

    m_dynamicsWorld->addRigidBody(m_groundRigidBody);

    m_fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)));

    m_fallShape = new btBoxShape(btVector3(0.1, 0.5, 0.1)); //
    m_fallShape = new btSphereShape(0.1);
    btScalar mass = 10;
    btVector3 m_fallInertia(0, 0, 0);
    m_fallShape->calculateLocalInertia(mass, m_fallInertia);

    btRigidBody::btRigidBodyConstructionInfo m_fallRigidBodyCI(mass, m_fallMotionState, m_fallShape, m_fallInertia);
    m_fallRigidBody = new btRigidBody(m_fallRigidBodyCI);
    m_dynamicsWorld->addRigidBody(m_fallRigidBody);
}

void Engine::doPhysics(double dt) {

    m_dynamicsWorld->stepSimulation(dt, 1000);

    btTransform trans;
    m_fallRigidBody->getMotionState()->getWorldTransform(trans);
    if (m_currentSelection != nullptr) {
        m_currentSelection->m_mesh->m_transform.pos() = {trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z()};
    }
}

void Engine::initializeSystems() {
    // Initialize Core system
    ecs::CoreSystem* coreSystem = new ecs::CoreSystem();
    m_systems.push_back(coreSystem);

    CameraController* cameraController = new CameraController();
    coreSystem->add(cameraController);

    // Initialize input system
    ecs::InputSystem *inputSystem = new ecs::InputSystem();
    m_systems.push_back(inputSystem);

    // Keyboard input
    KeyboardInput *keyboardInput = new KeyboardInput();
    inputSystem->add(keyboardInput);

    // Mouse input
    MouseInput *mouseInput = new MouseInput();
    inputSystem->add(mouseInput);

    // Picker component
    Picker *picker = new Picker();
    inputSystem->add(picker);

    // Joystick input
    JoystickInput *joystickInput = new JoystickInput();
    inputSystem->add(joystickInput);

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


void Engine::update() {
    m_timeDelta = glfwGetTime() - m_time;
    m_time = glfwGetTime();
    signal_engine_update.notify(m_time, m_timeDelta);
    glSampleCoverage(m_sample_coverage, GL_FALSE);
    doPhysics(m_timeDelta);
}

double Engine::time() {
    return glfwGetTime();
}

void Engine::renderScene() {
    if (m_scene != nullptr)
        m_scene->render(m_camera->viewProjection(), m_camera->view());
}

std::string stringify(float f) {
    std::ostringstream os;
    os << std::setprecision(3) << std::setw(7) << f;
    return os.str();
}

bool Engine::createFramebuffer() {
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &m_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);

    glDrawBuffer(GL_NONE); // No color buffer is drawn to.

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;
    return true;
}

void Engine::render() {
    m_framecounter++;
    m_window->start_rendering();
    check_gl_error();
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
    renderScene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    glm::mat4 viewmat = m_camera->view();
    MouseInput *mi = findComponent<MouseInput>();

    int samples;
    glGetIntegerv(GL_SAMPLES, &samples);


    std::ostringstream os;
    std::vector<float> axes = m_joystick != nullptr ? m_joystick->getJoystickAxes() : std::vector<float>({0, 0, 0, 0});
    os << "FPS: " << std::setprecision(3) << avg_fps << " [" << std::setprecision(2) << (1.0 / avg_fps) * 1000.0 <<
    " ms./frame]" << "\nFRAME:(" << m_framecounter << ")\nMEM:xxx MB"\
 << "\nCAMERA_ENABLED: [" << (mi->enabled() ? "YES" : "NO") << "] Key=TAB"\
 << "\nSAMPLES: [" << samples << "]"\
 << "\nJOYSTICK:[" << axes[0] << ", " << axes[1] << ", " << axes[2] << ", " << axes[3] << "]\n\n"\
 << "\nviewmatRIX :[" << stringify(viewmat[0][0]) << ", " << stringify(viewmat[0][1]) << ", " <<
    stringify(viewmat[0][2]) << ", " << stringify(viewmat[0][3]) << "]"\
 << "\n            [" << stringify(viewmat[1][0]) << ", " << stringify(viewmat[1][1]) << ", " <<
    stringify(viewmat[1][2]) << ", " << stringify(viewmat[1][3]) << "]"\
 << "\n            [" << stringify(viewmat[2][0]) << ", " << stringify(viewmat[2][1]) << ", " <<
    stringify(viewmat[2][2]) << ", " << stringify(viewmat[2][3]) << "]"\
 << "\n            [" << stringify(viewmat[3][0]) << ", " << stringify(viewmat[3][1]) << ", " <<
    stringify(viewmat[3][2]) << ", " << stringify(viewmat[3][3]) << "]";
    std::string text(os.str());
    m_text->render_text(text.c_str(), 14.0, -1 + 8 * sx, 1 - 14 * sy, sx, sy, glm::vec4(1, 1, 1, 1));

    m_window->end_rendering();

    handle_task_queue();
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    return m_window;
}

void Engine::keyHit(int key, int scanCode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
            exit(0);

        if (key == GLFW_KEY_TAB) {

            CameraController *cc = findComponent<CameraController>();
            if (cc->enabled()) {
                cc->setEnabled(false);
                m_window->showMouseCursor();
            }
            else {
                cc->setEnabled(true);
                m_window->hideMouseCursor();
            }

        }

        if (key == GLFW_KEY_M) {
            if (glIsEnabled(GL_MULTISAMPLE))
                glDisable(GL_MULTISAMPLE);
            else
                glEnable(GL_MULTISAMPLE);
        }

        if (key == GLFW_KEY_KP_ADD) {
            post(std::async([&](void) {
                m_sample_coverage = fmin(1.0, m_sample_coverage + .1);
            }));
        }

        if (key == GLFW_KEY_KP_SUBTRACT) {
            post(std::async([&](void) {
                m_sample_coverage = fmax(0.0, m_sample_coverage - .1);
            }));
        }

        std::cout << "You presssed: " << key << std::endl;
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

void Engine::post(std::future<void> task, double delay) {
    m_future_tasks.push_back({std::chrono::system_clock::now(), std::chrono::duration<double>(delay), std::move(task)});
}

void Engine::handle_task_queue() {
    // Run tasks that are not pending
    for (auto &task : m_future_tasks) {
        if (!task.pending())
            task.task.get();
    }
    // Erase tasks that are not pending
    auto i = m_future_tasks.begin();
    while (i != m_future_tasks.end()) {
        if (!i->pending())
            i = m_future_tasks.erase(i);
        ++i;
    }
}
