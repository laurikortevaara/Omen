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

	std::shared_ptr<MeshProvider> provider = std::make_shared<MeshProvider>();
	std::shared_ptr<Mesh> mesh = provider->createPlane();
	
	std::shared_ptr<omen::ecs::GameObject> obj = std::make_shared<omen::ecs::GameObject>("obj");
	std::shared_ptr<omen::ecs::MeshController> mc = std::make_shared<omen::ecs::MeshController>();
	std::shared_ptr<omen::ecs::MeshRenderer> mr = std::make_shared<omen::ecs::MeshRenderer>();
	obj->addCompnent(mc.get());
	obj->addCompnent(mr.get());
	mc->setMesh(mesh);
	addEntity(obj);
}

std::shared_ptr<Model> Scene::loadModel(const std::string filename) {
	omen::MD3Loader loader;
	loader.loadModel(filename);
	std::vector<std::shared_ptr<omen::Mesh>> meshes;
	loader.getMesh(meshes);
	int i = 0;
	std::shared_ptr<Model> model;
	for (auto& mesh : meshes) {
		model = std::make_shared<Model>(mesh);
		
		std::string name = filename;
		name += "_";
		name += i;
		std::shared_ptr<ecs::GameObject> obj = std::make_shared<ecs::GameObject>(name);
		std::shared_ptr<ecs::MeshController> meshController = std::make_shared<ecs::MeshController>();
		meshController->setMesh(model->mesh());
		obj->addComponent(meshController);

		addEntity(obj);
		
		i++;
	}
	return model;
}

Scene::~Scene() {
}

void Scene::render(const glm::mat4 &viewProjection, const glm::mat4 &view) {
	check_gl_error();
	ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();
	gs->render();
}

void omen::Scene::addEntity(std::shared_ptr<ecs::Entity> entity)
{
	m_entities.push_back(entity);
}
