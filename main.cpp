
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
#include "Editor/EditorWindow.h"
#include "Editor/GUILayout.h"

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
	omen::ecs::System *inputSystem = engine->findSystem<ecs::InputSystem>();

	/*check_gl_error();

	
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
	*/

	std::unique_ptr<GUILayout> gui = std::make_unique<GUILayout>();

	gui->addChild(std::move(std::make_unique<EditorWindow>("MainWindow")));
	gui->addChild(std::move(std::make_unique<EditorWindow>("MainWindow2")));

	Engine::instance()->scene()->addEntity(std::move(gui));

	/* Loop until the user closes the window */
	while (!window->shouldClose()) {
		//check_gl_error();
		engine->update();
		check_gl_error();
		engine->render();

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}