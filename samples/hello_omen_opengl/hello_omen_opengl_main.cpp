
#include <stdlib.h>
#include <memory>
#include <iostream>
#include "GL_error.h"
#include <time.h>
#include <mmsystem.h>
#include <Windows.h>
#include <GLFW\glfw3.h>

#include "Color.h"
#include "Scene.h"

#include "Window.h"
#include "Engine.h"
#include "system/InputSystem.h"

#include "component/KeyboardInput.h"
#include "Editor/EditorScene.h"
#include "Editor/EditorWindow.h"
#include "Editor/EditorCameraView.h"
#include "Editor/GUILayout.h"

#include "ui/LinearLayout.h"
#include "ui/Button.h"
#include "ui/TextView.h"

using namespace omen;
using namespace omen::ui;
using namespace omen::editor;

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
	/*
	std::unique_ptr<GUILayout> gui = std::make_unique<GUILayout>(LinearLayout::VERTICAL);
	gui->setBackground(Color("b0d4e1"));
	gui->setMargins(glm::vec4(100, 100, 100, 100));
	gui->addChild(std::make_unique<EditorCameraView>());
	*/
	////// SCENE
	std::unique_ptr<GUILayout> gui = std::make_unique<GUILayout>(LinearLayout::VERTICAL);
	gui->setBackground(Color("b0d4e1"));
	gui->setLayer(0);

	std::unique_ptr<LinearLayout> top = std::make_unique<LinearLayout>(nullptr, "Top", glm::vec2(0, 0), glm::vec2(0, 100), LinearLayout::HORIZONTAL);
	top->setBackground(Color("ff0000"));
	top->layoutParams().layoutSizingHeight = LayoutParams::Absolute;
	top->layoutParams().layoutSizingWidth = LayoutParams::MatchParent;

	top->addChild(std::make_unique<Button>(nullptr, "Button1", "textures/UI/button_normal.png"));
	top->addChild(std::make_unique<Button>(nullptr, "Button1", "textures/UI/button_normal.png"));
	top->addChild(std::make_unique<Button>(nullptr, "Button1", "textures/UI/button_normal.png"));
	top->addChild(std::make_unique<Button>(nullptr, "Button1", "textures/UI/button_normal.png"));
	top->addChild(std::make_unique<Button>(nullptr, "Button1", "textures/UI/button_normal.png"));


	std::unique_ptr<LinearLayout> bottom = std::make_unique<LinearLayout>(nullptr, "Bottom", glm::vec2(0, 0), glm::vec2(0, 0), LinearLayout::HORIZONTAL);
	bottom->layoutParams().layoutSizingHeight = LayoutParams::MatchParent;
	bottom->layoutParams().layoutSizingWidth = LayoutParams::MatchParent;
	bottom->setBackground(Color("#00ff00"));

	std::unique_ptr<LinearLayout> l1 = std::make_unique<LinearLayout>(nullptr, "Left", glm::vec2(0, 0), glm::vec2(0, 0));
	std::unique_ptr<LinearLayout> l2 = std::make_unique<LinearLayout>(nullptr, "Middle", glm::vec2(0, 0), glm::vec2(0, 0));
	std::unique_ptr<LinearLayout> l3 = std::make_unique<LinearLayout>(nullptr, "Right", glm::vec2(0, 0), glm::vec2(0, 0));
	l1->layoutParams().weight = 0.25f;
	l2->layoutParams().weight = 0.5f;
	l3->layoutParams().weight = 0.25f;

	//	l1->setBackground(Color("0098cb"));
	l1->setBackground(std::make_unique<ecs::Sprite>("textures/Background.png"));
	l2->setBackground(Color("0098cb"));
	l3->setBackground(Color("0098cb"));

	//bottom->setMargins(glm::vec4(10, 10, 10, 10));
	//bottom->setPaddings(glm::vec4(100, 100, 100, 100));

	l1->layoutParams().layoutSizingHeight = LayoutParams::MatchParent;
	l2->layoutParams().layoutSizingHeight = LayoutParams::MatchParent;
	l3->layoutParams().layoutSizingHeight = LayoutParams::MatchParent;

	l1->setLayer(1);
	l2->setLayer(1);
	l3->setLayer(1);

	l1->setMargins(glm::vec4(2, 2, 2, 2));
	l2->setMargins(glm::vec4(2, 2, 2, 2));
	l3->setMargins(glm::vec4(2, 2, 2, 2));
	l1->setPaddings(glm::vec4(20, 20, 20, 20));

	bottom->addChild(std::move(l1));
	bottom->addChild(std::move(l2));
	bottom->addChild(std::move(l3));

	gui->addChild(std::move(top));
	gui->addChild(std::move(bottom));

	View* left = dynamic_cast<View*>(gui->findChild("Left"));
	View* middle = dynamic_cast<View*>(gui->findChild("Middle"));
	View* right = dynamic_cast<View*>(gui->findChild("Right"));

	left->layoutParams().layoutSizingHeight = LayoutParams::MatchParent;
	middle->layoutParams().layoutSizingHeight = LayoutParams::MatchParent;
	right->layoutParams().layoutSizingHeight = LayoutParams::MatchParent;
	left->layoutParams().layoutSizingWidth = LayoutParams::MatchParent;
	middle->layoutParams().layoutSizingWidth = LayoutParams::MatchParent;
	right->layoutParams().layoutSizingWidth = LayoutParams::MatchParent;

	left->addChild(std::make_unique<TextView>(nullptr, "Text1", glm::vec2(0, 0), glm::vec2(0, 0)));
	dynamic_cast<TextView*>(left->findChild("Text1"))->setText(L"\
 Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed porta accumsan blandit. Morbi sagittis, enim et dapibus ultricies, justo massa sollicitudin felis, eu venenatis nisi metus ut mauris. Sed eu nibh sollicitudin, auctor purus volutpat, convallis dui. Sed suscipit sem urna. Aenean consequat, leo et auctor condimentum, odio sapien ultrices tellus, quis egestas quam nisi a arcu. Cras nulla est, volutpat at scelerisque ac, venenatis vitae massa. Cras nibh turpis, fringilla non vestibulum ac, ultricies nec justo. Cras venenatis tincidunt ante non condimentum. Maecenas turpis ante, luctus quis consectetur ut, auctor a orci.\
\
		Morbi mollis felis quis sapien semper, sodales placerat turpis suscipit.Nulla facilisi.Donec tortor ligula, laoreet eget tincidunt vitae, mollis id nibh.Duis ut mattis ligula, eget varius ligula.Proin eleifend eget orci vel pharetra.Duis ultrices interdum tincidunt.Pellentesque ut lacus ullamcorper, malesuada massa quis, vehicula mi.Quisque in orci vitae sem congue scelerisque vel facilisis magna.Maecenas maximus viverra fringilla.Cras luctus auctor dolor eget eleifend.Aenean eu eleifend ligula, vitae viverra leo.Vivamus quis felis sit amet augue consequat efficitur.Morbi a scelerisque arcu.\
\
		Nulla non iaculis elit, eget auctor eros.Aenean feugiat turpis eros, ac facilisis lectus auctor sed.Quisque scelerisque, ipsum at gravida congue, neque tortor mollis odio, id euismod quam sapien non nisl.Sed elementum placerat est eu laoreet.Quisque placerat dignissim arcu sed varius.Morbi vel leo vel mauris tincidunt vulputate.Maecenas viverra velit nec elit bibendum dictum.Ut mattis aliquet ligula, pellentesque ullamcorper mauris venenatis sed.Sed eget augue semper, vulputate mi vel, dapibus dolor.Proin et euismod velit, in rutrum mauris.Proin orci eros, elementum vel tellus non, ultrices porta sapien.Fusce libero sapien, feugiat eu lectus quis, varius pellentesque diam.");
	dynamic_cast<TextView*>(left->findChild("Text1"))->layoutParams().layoutSizingWidth = LayoutParams::MatchParent;
	dynamic_cast<TextView*>(left->findChild("Text1"))->layoutParams().layoutSizingHeight = LayoutParams::WrapContent;
	dynamic_cast<TextView*>(left->findChild("Text1"))->setMargins(glm::vec4(0, 10, 0, 10));

	left->addChild(std::make_unique<TextView>(nullptr, "Text2", glm::vec2(0, 0), glm::vec2(0, 0)));
	dynamic_cast<TextView*>(left->findChild("Text2"))->setText(L"abcdefghijklmnopqrstuvwxyzåäö - ABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ");
	dynamic_cast<TextView*>(left->findChild("Text2"))->layoutParams().layoutSizingWidth = LayoutParams::MatchParent;
	dynamic_cast<TextView*>(left->findChild("Text2"))->layoutParams().layoutSizingHeight = LayoutParams::WrapContent;
	dynamic_cast<TextView*>(left->findChild("Text2"))->setMargins(glm::vec4(0, 10, 0, 10));

	left->addChild(std::make_unique<TextView>(nullptr, "Text3", glm::vec2(0, 0), glm::vec2(0, 0)));
	dynamic_cast<TextView*>(left->findChild("Text3"))->setText(L"abcdefghijklmnopqrstuvwxyzåäö - ABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ");
	dynamic_cast<TextView*>(left->findChild("Text3"))->layoutParams().layoutSizingWidth = LayoutParams::MatchParent;
	dynamic_cast<TextView*>(left->findChild("Text3"))->layoutParams().layoutSizingHeight = LayoutParams::WrapContent;
	dynamic_cast<TextView*>(left->findChild("Text3"))->setMargins(glm::vec4(0, 10, 0, 10));

	middle->addChild(std::make_unique<EditorCameraView>());

	left->addChild(std::make_unique<Button>(nullptr, "Button1", "textures/UI/button_normal.png"));

	left->updateLayout();
	////// SCENE

	Engine::instance()->editorScene()->addEntity(std::move(gui));


	glfwSetWindowRefreshCallback(Engine::instance()->window()->window(), [](GLFWwindow* win) {
		Engine::instance()->render();
	});
	/* Loop until the user closes the window */
	while (!window->shouldClose()) {
		Engine::instance()->update();
		Engine::instance()->render();
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}