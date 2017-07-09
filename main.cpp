
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
#include "ui/LinearLayout.h"

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

	using namespace omen::ui;
	using namespace std;

	unique_ptr<LinearLayout> layout = make_unique<LinearLayout>(nullptr, "MainLayout", glm::vec2( { .0f,.0f } ), glm::vec2({ .0f,.0f }), LinearLayout::VERTICAL);
	unique_ptr<LinearLayout> toolbar = make_unique<LinearLayout>(nullptr, "ToolbarLayout", glm::vec2({ .0f,.0f }), glm::vec2({ .0f,.0f }), LinearLayout::HORIZONTAL);
	
	unique_ptr<ImageView> button1 = make_unique<ImageView>(nullptr, "Button1", "textures/UI/Button_Rect_Normal.png");
	unique_ptr<ImageView> button2 = make_unique<ImageView>(nullptr, "Button2", "textures/UI/CheckBox_On.png");
	unique_ptr<ImageView> button3 = make_unique<ImageView>(nullptr, "Button3", "textures/UI/Switch_On.png");
	unique_ptr<ImageView> button4 = make_unique<ImageView>(nullptr, "Button4", "textures/UI/Focus.png");
	toolbar->addChild(move(button1));
	toolbar->addChild(move(button2));
	toolbar->addChild(move(button3));
	toolbar->addChild(move(button4));

	unique_ptr<EditorWindow> w1 = make_unique<EditorWindow>("MainWindow");
	unique_ptr<EditorWindow> w2 = make_unique<EditorWindow>("MainWindow2");
	unique_ptr<EditorWindow> w3 = make_unique<EditorWindow>("MainWindow3");
	w1->setResizeMode(VERTICAL_RESIZE_ENABLED);
	w3->setResizeMode(VERTICAL_RESIZE_ENABLED);
	w1->setResizeMode(VERTICAL_RESIZE_ENABLED);

	layout->addChild(move(toolbar));
	layout->addChild(move(w1));
	layout->addChild(move(w3));

	gui->addChild(move(layout));
	gui->addChild(move(w2));

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