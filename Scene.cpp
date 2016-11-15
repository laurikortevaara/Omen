//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <iostream>
#include "Scene.h"
#include "Model.h"
#include "component/MeshController.h"
#include "utils.h"
#include "WavefrontLoader.h"
#include "GL_error.h"
#include "Engine.h"
#include "MD3Loader.h"
#include "PointLight.h"
#include "Ocean.h"
#include "system/GraphicsSystem.h"
#include "MeshProvider.h"
#include "GameObject.h"
#include "component/MeshRenderer.h"
#include "ui/Button.h"
#include "ui/Slider.h"
#include "ui/TextView.h"

using namespace omen;

Scene::Scene() {
	Engine* e = Engine::instance();
	Window* w = e->window();

	Engine::instance()->window()->signal_file_dropped.connect([this](std::vector<std::string> files)
	{
		loadModel(files.front());
	});

	JoystickInput* ji = (JoystickInput*)e->findComponent<JoystickInput>();
	if (ji != nullptr) {
		ji->joystick_button_pressed.connect([&](Joystick *joystick) {

		});
	}
}

std::unique_ptr<Model> Scene::loadModel(const std::string filename) {
	omen::MD3Loader loader;
	loader.loadModel(filename);
	std::vector<std::unique_ptr<omen::Mesh>> meshes;
	loader.getMesh(meshes);
	int i = 0;
	std::unique_ptr<Model> model;
	for (auto& mesh : meshes) {
		model = std::make_unique<Model>(std::move(mesh));
		
		std::string name = filename;
		name += "_";
		name += i;
		std::unique_ptr<ecs::GameObject> obj = std::make_unique<ecs::GameObject>(name);
		std::unique_ptr<ecs::MeshController> meshController = std::make_unique<ecs::MeshController>();
		//meshController->setMesh(model->mesh());
		//obj->addComponent(meshController);

		//addEntity(obj);
		
		i++;
	}
	return model;
}

Scene::~Scene() {
}

void omen::Scene::initialize()
{
	std::unique_ptr<MeshProvider> provider = std::make_unique<MeshProvider>();
	std::unique_ptr<Mesh> mesh = provider->createPlane();

	omen::ecs::GraphicsSystem *gs = omen::Engine::instance()->findSystem<omen::ecs::GraphicsSystem>();
	std::unique_ptr<omen::ecs::GameObject> obj = std::make_unique<omen::ecs::GameObject>("obj");
	std::unique_ptr<omen::ecs::MeshController> mc = std::make_unique<omen::ecs::MeshController>();
	mc->setMesh(std::move(mesh));

	std::unique_ptr<omen::ecs::MeshRenderer> mr = std::make_unique<omen::ecs::MeshRenderer>();

	obj->addCompnent(std::move(mc));
	obj->addCompnent(std::move(mr));

	addEntity(std::move(obj));

	for (int i = 0; i < 20; ++i){
		std::unique_ptr<ui::Slider> slider = std::make_unique<ui::Slider>(nullptr, "Slider"+std::to_string(i), "textures/slider_groove.png", glm::vec2(100, 100+i*25), 100+ omen::random(0, 900),10);
		addEntity(std::move(slider));
	}

	std::unique_ptr<ui::TextView> tv = std::make_unique<ui::TextView>(nullptr, "TextView");
	tv->setText(L"Textii :D");
	ui::TextView* ptr = tv.get();
	ui::Slider* e = dynamic_cast<ui::Slider*>(findEntity("Slider1"));
	if(e!=nullptr) e->signal_slider_dragged.connect([ptr](ui::Slider* slider, float value) -> void {
		std::wstring str = L"Slider Dragged:";
		str += omen::to_wstring_with_precision(value, 2);
		ptr->setText(str);
	});
	addEntity(std::move(tv));
}

void Scene::render(const glm::mat4 &viewProjection, const glm::mat4 &view) {
	check_gl_error();
	ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();
	gs->render();
}

void omen::Scene::addEntity(std::unique_ptr<ecs::Entity> entity)
{
	ecs::Entity* eptr = entity.get();
	m_entities.push_back(std::move(entity));
	signal_entity_added.notify(eptr);
}

ecs::Entity* Scene::findEntity(const std::string& name)
{
	for (const auto& e : entities()) {
		if (e->name() == name)
			return e.get();
		ecs::Entity* c = e->findChild(name);
		if (c != nullptr)
			return c;
	}
	return nullptr;
}