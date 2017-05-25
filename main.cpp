
#include <stdlib.h>
#include <memory>
#include <iostream>
#include "Scene.h"
#include "Window.h"
#include "Engine.h"
#include "system/InputSystem.h"
#include "component/KeyboardInput.h"
#include "GL_error.h"
#include <time.h>
#include <mmsystem.h>
#include <Windows.h>
#include <GLFW\glfw3.h>

using namespace omen;

/**
* the main
**/
int main(int argc, char *argv[]) 
{
	/* initialize random seed: */
	srand(unsigned int(time(nullptr)));
	omen::Engine *engine = omen::Engine::instance();
	const std::unique_ptr<omen::Window>& window = engine->createWindow(1000,1000);
	check_gl_error();

	omen::ecs::System *inputSystem = engine->findSystem<ecs::InputSystem>();
	check_gl_error();
	if (inputSystem != nullptr) {
		omen::KeyboardInput *keyboardInput = dynamic_cast<omen::KeyboardInput *>(inputSystem->findComponent<omen::KeyboardInput>());


		keyboardInput->signal_key_press.connect_static([&](int k, int s, int a, int m) {
			// TODO
		});

		keyboardInput->signal_key_release.connect_static([&](int k, int s, int a, int m) {
			// TODO
		});
	}
		
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