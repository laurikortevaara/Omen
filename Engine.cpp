//
// Created by Lauri Kortevaara(personal) on 20/12/15.
//


#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <sstream>
#include <iomanip>
#include <queue>
#include <png.h>

#include "Engine.h"
#include "GL_error.h"
#include "utils.h"
#include "thirdparty/glfw/include/GLFW/glfw3.h"
#include "MD3Loader.h"
#include "system/CoreSystem.h"
#include "system/InputSystem.h"
#include "component/KeyboardInput.h"
#include "component/MouseInput.h"
#include "component/Picker.h"
#include "component/CameraController.h"
#include "component/Transformer.h"
#include "system/GraphicsSystem.h"


using namespace omen;

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
        m_currentSelection(nullptr),
        m_polygonMode(GL_FILL) {

    std::string currentDir = omen::getWorkingDir();
    if (currentDir.find("bin") == std::string::npos)
        chdir("bin");

    Window::signal_window_created.connect([this](std::shared_ptr<Window> window) {
        m_window = window;

        initializeSystems();

        m_camera = new Camera("Camera1", {0, 5, 10}, {0, 0, 0}, 45.0f);
        findComponent<CameraController>()->setCamera(m_camera);
        m_scene = std::make_unique<Scene>();
        m_text = std::make_unique<TextRenderer>();

        createFramebuffer();

        omen::MD3Loader loader;
        loader.loadModel("models/sphere.md3");
        std::vector<std::shared_ptr<omen::Mesh>> meshes;
        for (int i = 0; i < 1; ++i) {
            loader.getMesh(meshes);
            m_currentSelection = nullptr;
            std::shared_ptr<Model> model = std::make_shared<Model>(meshes.front());
            m_currentSelection = model.get();
            model->m_mesh->m_amplitude = 0.0;
            model->m_mesh->m_transform.pos() = {omen::random(-10, 10), omen::random(1, 5), omen::random(-10, 10)};
            model->m_mesh->m_transform.scale({omen::random(1, 4), omen::random(1, 4), omen::random(1, 4)});
            m_scene->models().push_back(model);
            meshes.clear();
        }

        initPhysics();

        //m_button = new Button("Button1");
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

    float rest = 0.4;
    float friction = 1.05;
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, m_groundMotionState, m_groundShape,
                                                               btVector3(0, 0, 0));
    groundRigidBodyCI.m_restitution = rest;
    groundRigidBodyCI.m_friction = friction;
    m_groundRigidBody = new btRigidBody(groundRigidBodyCI);

    m_dynamicsWorld->addRigidBody(m_groundRigidBody);

    m_fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)));

    m_fallShape = new btBoxShape(btVector3(0.1, 0.1, 0.1)); //
    m_fallShape = new btSphereShape(0.1);
    btScalar mass = 10;
    btVector3 m_fallInertia(0, 0, 0);
    m_fallShape->calculateLocalInertia(mass, m_fallInertia);

    btRigidBody::btRigidBodyConstructionInfo m_fallRigidBodyCI(mass, m_fallMotionState, m_fallShape, m_fallInertia);
    m_fallRigidBodyCI.m_restitution = rest;
    m_fallRigidBodyCI.m_friction = friction;
    m_fallRigidBody = new btRigidBody(m_fallRigidBodyCI);
    m_dynamicsWorld->addRigidBody(m_fallRigidBody);
}

void Engine::doPhysics(double dt) {

    m_dynamicsWorld->stepSimulation(dt, 1000);

    btTransform trans;
    m_fallRigidBody->getMotionState()->getWorldTransform(trans);
    if (m_currentSelection != nullptr) {
        m_currentSelection->m_mesh->m_transform.pos() = {trans.getOrigin().x(), trans.getOrigin().y(),
                                                         trans.getOrigin().z()};
    }
}

void Engine::initializeSystems() {
    // Initialize Core system
    ecs::CoreSystem *coreSystem = new ecs::CoreSystem();
    m_systems.push_back(coreSystem);

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

    keyboardInput->signal_key_release.connect([this](int k, int s, int a, int m) {
        if (k == GLFW_KEY_T)
            m_polygonMode = m_polygonMode == GL_FILL ? GL_LINE : GL_FILL;
    });

    // Connect key-hit, -press and -release signals to observers
    m_window->signal_window_size_changed.connect([this](int width, int height) {
        m_camera->onWindowSizeChanged(width, height);
    });


    CameraController *cameraController = new CameraController();
    coreSystem->add(cameraController);

    omen::Transformer *transformer = new Transformer();

    ecs::GraphicsSystem *graphicsSystem = new ecs::GraphicsSystem();
    m_systems.push_back(graphicsSystem);

}


void Engine::update() {
    m_timeDelta = glfwGetTime() - m_time;
    m_time = glfwGetTime();
    signal_engine_update.notify(m_time, m_timeDelta);
    //glSampleCoverage(m_sample_coverage, GL_FALSE);
    //doPhysics(m_timeDelta);
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
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_window->width(), m_window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorTexture, 0);

    glDrawBuffer(GL_BACK);

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    int w, h;
    int miplevel = 0;
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

bool Engine::createShadowFramebuffer() {
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    int w, h;
    int miplevel = 0;
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void Engine::render() {
    m_framecounter++;
    m_window->start_rendering();
    check_gl_error();
    //glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
    //renderScene();
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    renderScene();
    check_gl_error();

    //renderText();
    m_window->end_rendering();

    handle_task_queue();
}


std::shared_ptr<Window> Engine::createWindow(unsigned int width, unsigned int height) {
    m_window = std::make_shared<Window>();
    m_window->createWindow(width, height);
    check_gl_error();
    glFrontFace(GL_CCW);
    check_gl_error();
    glEnable(GL_CULL_FACE);
    check_gl_error();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    return m_window;
}

void abort_(const char *s, ...) {
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}


void write_png_file(const char *file_name, char *pixels, unsigned long width, unsigned long height) {
    png_byte color_type = PNG_COLOR_TYPE_RGBA;
    png_byte bit_depth = 8;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;


    /* create file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp)
        abort_("[write_png_file] File %s could not be opened for writing", file_name);


    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
        abort_("[write_png_file] png_create_write_struct failed");

    ///
    row_pointers = static_cast<png_bytepp>(png_malloc(png_ptr, height * sizeof(png_bytep)));
    for (unsigned long i = 0; i < height; i++)
        row_pointers[i] = static_cast<png_bytep>(png_malloc(png_ptr, width * 4));

    for (int y = 0; y < height; ++y) {
        row_pointers[y] = (png_bytep) (pixels + (height - y - 1) * width * 4);
    }
    ///

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("[write_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during init_io");

    png_init_io(png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing header");

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing bytes");

    png_write_image(png_ptr, row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during end of write");

    png_write_end(png_ptr, NULL);

    /* cleanup heap allocation */
    for (int y = 0; y < height; y++)
        png_free_ptr(row_pointers[y]);

    free(row_pointers);

    fclose(fp);
}

void Engine::keyHit(int key, int scanCode, int action, int mods) {
    if (mods == GLFW_MOD_SHIFT) {
        if (key == GLFW_KEY_W && action == GLFW_RELEASE) {

            //glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
            renderScene();

            int w, h;
            int miplevel = 0;
            glBindTexture(GL_TEXTURE_2D, m_colorTexture);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);
            glBindTexture(GL_TEXTURE_2D, 0);
            w = m_window->frameBufferWidth();
            h = m_window->frameBufferHeight();
            char *buf = new char[w * h * 4];
            glBindTexture(GL_TEXTURE_2D, 0);

            //glReadBuffer((GLenum) GL_COLOR_ATTACHMENT0);
            glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *) buf);

            write_png_file("test.png", buf, w, h);
            delete[] buf;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    else if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q)
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

Scene *Engine::scene() {
    return m_scene.get();
}

void Engine::renderText() {
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


    Picker *p = findComponent<Picker>();
    std::wostringstream os;
    std::vector<float> axes = m_joystick != nullptr ? m_joystick->getJoystickAxes() : std::vector<float>({0, 0, 0, 0});
    /*  os << "FPS: " << std::setprecision(3) << avg_fps << " [" << std::setprecision(2) << (1.0 / avg_fps) * 1000.0 <<
      " ms./frame]" << "\nFRAME:(" << m_framecounter << ")\nMEM:xxx MB"\
   << "\nCAMERA_ENABLED: [" << (mi->enabled() ? "YES" : "NO") << "] Key=TAB"\
   << "\nSAMPLES: [" << samples << "]"\
   << "\nJOYSTICK:[" << axes[0] << ", " << axes[1] << ", " << axes[2] << ", " << axes[3] << "]\n\n"\
   << "\nviewmatrix :[" << stringify(viewmat[0][0]) << ", " << stringify(viewmat[0][1]) << ", " <<
      stringify(viewmat[0][2]) << ", " << stringify(viewmat[0][3]) << "]"\
   << "\n            [" << stringify(viewmat[1][0]) << ", " << stringify(viewmat[1][1]) << ", " <<
      stringify(viewmat[1][2]) << ", " << stringify(viewmat[1][3]) << "]"\
   << "\n            [" << stringify(viewmat[2][0]) << ", " << stringify(viewmat[2][1]) << ", " <<
      stringify(viewmat[2][2]) << ", " << stringify(viewmat[2][3]) << "]"\
   << "\n            [" << stringify(viewmat[3][0]) << ", " << stringify(viewmat[3][1]) << ", " <<
      stringify(viewmat[3][2]) << ", " << stringify(viewmat[3][3]) << "]"\
  << "\nPickRay: [" << p->ray().x << ", " << p->ray().y << ", " << p->ray().z << "]";

  */
    os << "FPS: " << std::setprecision(3) <<
    avg_fps; // << " [" << std::setprecision(2) << (1.0 / avg_fps) * 1000.0 << " ms./frame]\n";
    /*int i=1;
    for(int y=0; y < 40; ++y ) {
        for (int x = 0; x < 128; ++x){
            if(std::isprint(i))
                os << wchar_t(i);
            ++i;
        }
        os << "\n";
    }*/
    std::wstring text(os.str());
    m_text->render_text(text.c_str(), 16.0, -1 + 8 * sx, 1 - 14 * sy, sx, sy, glm::vec4(1, 1, 1, 1));
}

GLenum Engine::getPolygonMode() {
    return m_polygonMode;
}
