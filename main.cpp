#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <memory>
#include <iostream>
#include "Scene.h"
#include "Window.h"
#include "Engine.h"
#include "system/InputSystem.h"
#include "component/KeyboardInput.h"

using namespace Omen;

std::shared_ptr<Omen::Scene> gScene = nullptr;

/**
* the main
**/
int main(int argc, char *argv[]) {

    Omen::Engine *engine = Omen::Engine::instance();
    Omen::Window *window = engine->createWindow(1280, 720);

    Omen::ecs::System *inputSystem = engine->findSystem<Omen::InputSystem>();
    if (inputSystem != nullptr) {
        Omen::KeyboardInput *keyboardInput = dynamic_cast<Omen::KeyboardInput *>(inputSystem->findComponent<Omen::KeyboardInput>());


        keyboardInput->signal_key_press.connect([&](int k, int s, int a, int m) {
            std::cout << "Foo" << std::endl;
        });

        keyboardInput->signal_key_release.connect([&](int k, int s, int a, int m) {
            std::cout << "Bar" << std::endl;
        });
    }


    //gScene = std::make_shared<Scene>(Scene());

    /* Loop until the user closes the window */
    while (!window->shouldClose()) {
        engine->update();
        engine->render();
    }

    glfwTerminate();
    return 0;
}