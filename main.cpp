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

using namespace Omen;

std::shared_ptr<Omen::Scene> gScene = nullptr;

/**
* the main
**/
int main(int argc, char *argv[]) {
    /* initialize random seed: */
    srand (time(NULL));
    Omen::Engine *engine = Omen::Engine::instance();
    std::shared_ptr<Omen::Window> window = engine->createWindow(1280, 720);
    check_gl_error();

    Omen::ecs::System *inputSystem = engine->findSystem<Omen::InputSystem>();
    check_gl_error();
    if (inputSystem != nullptr) {
        Omen::KeyboardInput *keyboardInput = dynamic_cast<Omen::KeyboardInput *>(inputSystem->findComponent<Omen::KeyboardInput>());


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