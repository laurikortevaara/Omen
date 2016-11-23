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
#include "Texture.h"

using namespace omen;

Shader* bgShader = nullptr;
omen::Texture* bgTexture = nullptr;

Scene::Scene() {
	Engine* e = Engine::instance();
	Window* w = e->window();

	bgShader = new Shader("shaders/texture_quad.glsl");
	bgTexture = new Texture("textures/bg_gradient2.jpg");

	Engine::instance()->window()->signal_file_dropped.connect([this](std::vector<std::string>& files)
	{
		std::string& file = *files.begin();
		if (file.find(".MD3") != std::string::npos 
			|| file.find(".md3") != std::string::npos) {
			loadModel(file);
		}
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
	for (auto& mesh : meshes) {
		std::string name = filename;
		name += "_";
		name += i;
		//std::unique_ptr<ecs::GameObject> obj = std::make_unique<ecs::GameObject>(name);
		std::unique_ptr<ecs::GameObject> obj = std::make_unique<ecs::GameObject>("OBJECT");
		std::unique_ptr<ecs::MeshController> meshController = std::make_unique<ecs::MeshController>();
		meshController->setMesh(std::move(mesh));
		obj->addComponent(std::move(meshController));
		std::unique_ptr<omen::ecs::MeshRenderer> mr = std::make_unique<omen::ecs::MeshRenderer>();
		obj->addCompnent(std::move(mr));
		addEntity(std::move(obj));
		
		i++;
	}
	meshes.clear();
	return nullptr;
}

Scene::~Scene() {
}

std::unique_ptr<ecs::GameObject> createObject() {
	std::unique_ptr<MeshProvider> provider = std::make_unique<MeshProvider>();

	std::unique_ptr<Mesh> mesh = provider->createPlane();
	std::unique_ptr<omen::ecs::GameObject> obj = std::make_unique<omen::ecs::GameObject>("obj");
	std::unique_ptr<omen::ecs::MeshController> mc = std::make_unique<omen::ecs::MeshController>();
	mc->setMesh(std::move(mesh));

	std::unique_ptr<Mesh> cube = provider->createCube();
	std::unique_ptr<omen::ecs::MeshController> mc2 = std::make_unique<omen::ecs::MeshController>();
	mc2->setMesh(std::move(cube));

	std::unique_ptr<omen::ecs::MeshRenderer> mr = std::make_unique<omen::ecs::MeshRenderer>();

	//obj->addCompnent(std::move(mc));
	obj->addCompnent(std::move(mc2));
	obj->addCompnent(std::move(mr));

	return obj;
}
void omen::Scene::initialize()
{
	for (int i = 0; i < 1; ++i){
		std::unique_ptr<ecs::GameObject> obj = createObject();
		obj->transform()->pos().x += i*2.1f;
		addEntity(std::move(obj));
	}
	
	for (int i = 0; i < 5; ++i){
		std::unique_ptr<ui::Slider> slider = std::make_unique<ui::Slider>(nullptr, "Slider"+std::to_string(i), "textures/slider_groove.png", glm::vec2(10, 100+i*20), 100,10);
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

void Scene::render(const glm::mat4 &viewProjection, const glm::mat4 &view) 
{
	renderBackground();
	check_gl_error();
	ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();
	gs->render();
}

void Scene::renderBackground()
{
	bgTexture->bind();
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	glViewport(0, 0, w, h);

	bgShader->use();

	glm::vec2 v[4] = { {-1.0,1.0},{-1.0,-1.0},{ 1.0, 1.0 },{ 1.0,-1.0 } };
	GLuint vbo = 0, vao = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &vbo);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDeleteBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);

	
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