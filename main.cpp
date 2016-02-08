#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <memory>
#include <iostream>
#include "Scene.h"
#include "Window.h"
#include "Engine.h"
#include "system/InputSystem.h"
#include "component/KeyboardInput.h"
#include "GL_error.h"

using namespace omen;

std::shared_ptr<omen::Scene> gScene = nullptr;

/**
* the main
**/
int main(int argc, char *argv[]) {
    /* initialize random seed: */
    srand (time(NULL));
    omen::Engine *engine = omen::Engine::instance();
    std::shared_ptr<omen::Window> window = engine->createWindow(1920, 1080);
    check_gl_error();

    omen::ecs::System *inputSystem = engine->findSystem<ecs::InputSystem>();
    check_gl_error();
    if (inputSystem != nullptr) {
        omen::KeyboardInput *keyboardInput = dynamic_cast<omen::KeyboardInput *>(inputSystem->findComponent<omen::KeyboardInput>());


        keyboardInput->signal_key_press.connect([&](int k, int s, int a, int m) {
            // TODO
        });

        keyboardInput->signal_key_release.connect([&](int k, int s, int a, int m) {
            // TODO
        });
    }


    //gScene = std::make_shared<Scene>(Scene());

    /* Loop until the user closes the window */
    while (!window->shouldClose()) {
        check_gl_error();
        engine->update();
        check_gl_error();
        engine->render();
    }

    glfwTerminate();
    return 0;
}