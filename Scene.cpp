//
// Created by Lauri Kortevaara on 08/11/15.
//

#include <iostream>
#include <vector>
#include <filesystem>

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
#include "ToolView.h"
#include "ui/TextView.h"
#include "Texture.h"
#include "component/MouseInput.h"
#include "RenderBuffer.h"
#include "MultipassShader.h"
#include "MathUtils.h"
#include "component/KeyboardInput.h"
#include "Terrain.h"
#include "Sky.h"
#include "ShadowMap.h"

using namespace omen;

Shader* quadShader = nullptr;
Shader* lineShader = nullptr;
Shader* bgShaderPass1 = nullptr;
Shader* bgShaderPass2 = nullptr;
Shader* bgShaderPass3 = nullptr;
Texture* bgTexture = nullptr;
RenderBuffer* renderBuffer = nullptr;
RenderBuffer* renderBuffer2 = nullptr;
MultipassShader* ms = nullptr;
float blur = 0.0f;
ShadowMap* shadowMap = nullptr;
ecs::GameObject* lightobj = nullptr;

namespace fs = std::experimental::filesystem;

Scene::Scene() {
	
	Engine* e = Engine::instance();
	Window* w = e->window();

	// Two pass gauss blur for the background
	quadShader = new Shader("shaders/quad.glsl");
	lineShader = new Shader("shaders/line_shader.glsl");
	bgShaderPass1 = new Shader("shaders/texture_quad.glsl");
	bgShaderPass2 = new Shader("shaders/ring_twisterdistance_field_text.glsl");

	//bgShaderPass3 = new Shader("shaders/physical_chromatic_aberration.glsl");
	renderBuffer = new RenderBuffer();
	renderBuffer2 = new RenderBuffer();

	ms = new MultipassShader();
	ms->addPass(MultipassShader::RenderPass(bgShaderPass1, [&](MultipassShader* multipass, Shader* shader, unsigned int pass) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int textureMapLoc = shader->getUniformLocation("Texture");
		glUniform1i(textureMapLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		bgTexture->bind();

		int w, h;
		glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
		glViewport(0, 0, w, h);

		bgShaderPass1->use();
		bgShaderPass1->setUniform1f("Blur", blur);

		glm::vec2 v[4] = { { -1.0,1.0 },{ -1.0,-1.0 },{ 1.0, 1.0 },{ 1.0,-1.0 } };
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

	}));
	ms->addPass(MultipassShader::RenderPass(bgShaderPass1, [&](MultipassShader* multipass, Shader* shader, unsigned int pass) {
		glClear(GL_DEPTH_BUFFER_BIT);
		ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();
		glClear(GL_DEPTH_BUFFER_BIT);
		gs->render(nullptr, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		gs->render(nullptr, 1);
		glClear(GL_DEPTH_BUFFER_BIT);
		gs->render(nullptr, 2);
		glClear(GL_DEPTH_BUFFER_BIT);
		gs->render(nullptr, 3);
	}));
	

	
	// Random background image
	std::vector<std::string> bgFiles;
	for (auto& filename : fs::directory_iterator("textures/")) {
		fs::directory_entry e(filename);
		std::ostringstream os;
		os << e;
		bgFiles.push_back(os.str());
	}
	//bgTexture = new Texture(bgFiles.at(omen::random(0,bgFiles.size()-1)));
	bgTexture = new Texture("textures/bg_gradient5.jpg");
	
	Engine::instance()->window()->signal_file_dropped.connect([this](const std::vector<std::string>& files)
	{
		const std::string& file = *files.begin();
		if (file.find(".MD3") != std::string::npos 
			|| file.find(".md3") != std::string::npos) {
			loadModel(file);
		}
		else
			if (file.find(".FBX") != std::string::npos
				|| file.find(".fbx") != std::string::npos) {
				std::unique_ptr<ecs::GameObject> obj = createObject(file);

				if(false)
				{
					std::unique_ptr<omen::ecs::GameObject> obj = std::make_unique<omen::ecs::GameObject>("obj");

					std::unique_ptr<omen::ecs::MeshController> mc = std::make_unique<omen::ecs::MeshController>();
					mc->setMesh(std::move(MeshProvider::createPlane(10000)));
					std::unique_ptr<omen::ecs::MeshRenderer> mr = std::make_unique<omen::ecs::MeshRenderer>(mc.get());
					obj->addCompnent(std::move(mr));
					obj->addCompnent(std::move(mc));

					obj->tr()->pos().x = -5000;
					obj->tr()->pos().y = -2;
					obj->tr()->pos().z = -5000;
					obj->setName("plane");
					/*Engine::instance()->signal_engine_update.connect([file](float time, float delta_time) {
					ecs::GameObject* obj = dynamic_cast<ecs::GameObject*>(Engine::instance()->scene()->findEntity(file));
					obj->tr()->rotate(time, glm::vec3(0.75, 0, 0));
					obj->tr()->rotate(time, glm::vec3(0, 0.34, 0));
					});*/
					addEntity(std::move(obj));
				}
			}
			else if (file.find(".JPG") != std::string::npos
				|| file.find(".jpg") != std::string::npos) {
				Texture* t = new Texture(file);
			}
	});

	JoystickInput* ji = (JoystickInput*)e->findComponent<JoystickInput>();
	if (ji != nullptr) {
		ji->joystick_button_pressed.connect([&](Joystick *joystick) {

		});
	}

	MouseInput *mi = Engine::instance()->findComponent<MouseInput>();
	if (mi != nullptr) {
		mi->signal_mouse_scrolled.connect([&](omen::floatprec x, omen::floatprec y) {
			blur += y;
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
		std::unique_ptr<ecs::MeshController> mc = std::make_unique<ecs::MeshController>();
		mc->setMesh(std::move(mesh));
		std::unique_ptr<omen::ecs::MeshRenderer> mr = std::make_unique<omen::ecs::MeshRenderer>(mc.get());
		obj->addCompnent(std::move(mr));
		obj->addComponent(std::move(mc));
		
		//addEntity(std::move(obj));
		
		i++;
	}
	meshes.clear();
	return nullptr;
}

Scene::~Scene() {
}

std::unique_ptr<ecs::GameObject> Scene::createObject(const std::string& filename ) {
	std::unique_ptr<MeshProvider> provider = std::make_unique<MeshProvider>();

	std::list< std::unique_ptr<omen::ecs::GameObject> > meshes = provider->loadObject(filename);
	int i = 0;
	for (auto& obj : meshes)
	{
		/*Engine::instance()->signal_engine_update.connect([file](float time, float delta_time) {
		ecs::GameObject* obj = dynamic_cast<ecs::GameObject*>(Engine::instance()->scene()->findEntity(file));
		obj->tr()->rotate(time, glm::vec3(0.75, 0, 0));
		obj->tr()->rotate(time, glm::vec3(0, 0.34, 0));
		});*/
		addEntity(std::move(obj));
	}
	
	return nullptr;
}
void omen::Scene::initialize()
{
	float distributionFactor = 10.0f;

	for (int i = 0; i < 10; ++i){
		std::unique_ptr<ui::Slider> slider = std::make_unique<ui::Slider>(nullptr, "Slider"+std::to_string(i+1), "textures/slider_groove.png", glm::vec2(10, 100+i*20), glm::vec2(500,10));
		addEntity(std::move(slider), 1);
	}

	std::unique_ptr<ui::TextView> tv = std::make_unique<ui::TextView>(nullptr, "TextView", glm::vec2(0, 150), glm::vec2(200, 200));
	tv->setText(L"Textii :D");
	addEntity(std::move(tv), 3);

	ui::Slider* e = dynamic_cast<ui::Slider*>(findEntity("Slider1"));
	e->setPos(Engine::ShadowFrustumNear/10.0f);
	if(e!=nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		//std::wstring str = L"Slider Dragged:";
		//str += omen::to_wstring_with_precision(value, 2);
		//ptr->setText(str);
		Engine::ShadowFrustumNear = 10.0f*value;
	});

	e = dynamic_cast<ui::Slider*>(findEntity("Slider2"));
	e->setPos(Engine::ShadowFrustumFar / 8000.0f);
	if (e != nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		Engine::ShadowFrustumFar = 8000.0f*value;
	});

	e = dynamic_cast<ui::Slider*>(findEntity("Slider3"));
	e->setPos(Engine::ShadowFrustumSize / 2000.0f);
	if (e != nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		Engine::ShadowFrustumSize = 2000.0f*value;
	});

	e = dynamic_cast<ui::Slider*>(findEntity("Slider4"));
	e->setPos(Engine::LightDistance);
	if (e != nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		Engine::LightDistance = value;
	});

	e = dynamic_cast<ui::Slider*>(findEntity("Slider5"));
	e->setPos(Engine::LightDistance);
	if (e != nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		Engine::ShadowBlur = value*10.0f;
	});
	
	e = dynamic_cast<ui::Slider*>(findEntity("Slider6"));
	e->setPos(Engine::LightIntensity);
	if (e != nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		Engine::LightIntensity = value*10.0;
	});

	e = dynamic_cast<ui::Slider*>(findEntity("Slider7"));
	e->setPos(Engine::AmbientFactor);
	if (e != nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		Engine::AmbientFactor = value;
	});
	
	e = dynamic_cast<ui::Slider*>(findEntity("Slider8"));
	e->setPos(Engine::MaterialShininess);
	if (e != nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		Engine::MaterialShininess = value*100.0f;
	});

	e = dynamic_cast<ui::Slider*>(findEntity("Slider9"));
	e->setPos(Engine::MaterialShininess);
	if (e != nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		Engine::LightAzimuthAngle = value*360.0f;
	});

	e = dynamic_cast<ui::Slider*>(findEntity("Slider10"));
	e->setPos(Engine::MaterialShininess);
	if (e != nullptr) e->signal_slider_dragged.connect([](ui::Slider* slider, float value) -> void {
		Engine::LightZenithAngle = value*90.0f;
	});


	/*
	std::unique_ptr<ui::Slider> slider = std::make_unique<ui::Slider>(nullptr, "Slider", "textures/slider_groove.png", glm::vec2(100, 100), glm::vec2(100, 100));
	ui::Slider* ptr = slider.get();
	ptr->signal_slider_dragged.connect([ptr](ui::Slider* slider, float value) -> void {
		std::wstring str = L"Slider Dragged:";
		str += omen::to_wstring_with_precision(value, 2);
		std::wcout << str << std::endl;
	});
	addEntity(std::move(slider));*/
	
	Engine::instance()->findComponent<KeyboardInput>()->signal_key_press.connect([this](int key, int scan, int action, int mods)
	{
		if (key == GLFW_KEY_C && mods & GLFW_MOD_SHIFT) {
			Scene* s = this;
			s->m_entities.clear();
		}
	});

	//std::unique_ptr<ui::TextView> tv = std::make_unique<ui::TextView>(nullptr, "FPS_COUNTER");
	Engine::instance()->signal_engine_update.connect([](float time, float delta_time) {
		if (lightobj != nullptr) {
			lightobj->tr()->pos() = Engine::LightPos;
		}

		ecs::GameObject* sphere = dynamic_cast<ecs::GameObject*>(Engine::instance()->scene()->findEntity("Sphere"));
		if (sphere != nullptr)
		{
			Transform* tr = sphere->getComponent<Transform>();
			float t = Engine::instance()->time();
			float d = 10.0 + sin(cos(t*0.5)*3.14)*cos(t*0.5)*15.0;
			tr->pos() = glm::vec3(sin(t)*d, 0.4+sin(t), cos(t)*d);
		}

		ui::TextView* obj = dynamic_cast<ui::TextView*>(Engine::instance()->scene()->findEntity("FPS_COUNTER"));
		if (obj != nullptr) {
			std::setprecision(2);
			glm::vec3 v1 = glm::vec3(Engine::instance()->camera()->view()[0][0], Engine::instance()->camera()->view()[0][1], Engine::instance()->camera()->view()[0][2]);
			glm::vec3 v2 = glm::vec3(Engine::instance()->camera()->view()[1][0], Engine::instance()->camera()->view()[1][1], Engine::instance()->camera()->view()[1][2]);
			glm::vec3 v3 = glm::vec3(Engine::instance()->camera()->view()[2][0], Engine::instance()->camera()->view()[2][1], Engine::instance()->camera()->view()[2][2]);
			float y_angle = atan2(Engine::instance()->camera()->view()[0][0], Engine::instance()->camera()->view()[0][2]) * 180 / M_PI;
			std::wstring fps_str = L"FPS:" + omen::to_wstring_with_precision(Engine::instance()->averageFps(), 3) +
				L"\nCameraPos: [" + omen::to_wstring_with_precision(Engine::instance()->camera()->position().x, 1) +
				L", " + omen::to_wstring_with_precision(Engine::instance()->camera()->position().y, 1) +
				L", " + omen::to_wstring_with_precision(Engine::instance()->camera()->position().z, 1) + L"]" +
				L"\nCameraDir: [" + omen::to_wstring_with_precision(y_angle, 3) + L"]";
			obj->setText(fps_str.c_str());
		}
	});
	//addEntity(std::move(tv));


	//std::unique_ptr<MeshProvider> provider = std::make_unique<MeshProvider>();

	/*std::unique_ptr<Mesh> mesh = provider->createPlane();
	std::unique_ptr<omen::ecs::GameObject> obj = std::make_unique<omen::ecs::GameObject>("plane");
	std::unique_ptr<omen::ecs::MeshController> mc = std::make_unique<omen::ecs::MeshController>();
	mc->setMesh(std::move(mesh));

	std::unique_ptr<omen::ecs::MeshRenderer> mr = std::make_unique<omen::ecs::MeshRenderer>(mc.get());
	obj->addCompnent(std::move(mr));
	obj->addCompnent(std::move(mc));

	

	addEntity(std::move(obj));*/
	//addEntity(std::make_unique<Terrain>());
	//addEntity(std::make_unique<Sky>());
	shadowMap = new ShadowMap();
	shadowMap->init();

	/*std::unique_ptr<MeshProvider> provider = std::make_unique<MeshProvider>();
	std::unique_ptr<ecs::GameObject> l = std::move(provider->loadObject("models/light.fbx").begin().operator*());
	lightobj = l.get();
	lightobj->getComponent<omen::ecs::MeshController>()->setCastShadow(false);
	addEntity(std::move(l));*/

	addEntity(std::make_unique<omen::ui::ToolView>("Tools", "texture/toolbar.jpg", glm::vec2(200, 200), glm::vec2( 200, 200)));
}

void Scene::render(const glm::mat4 &viewProjection, const glm::mat4 &view) 
{
	ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();

	shadowMap->render();
	glEnable(GL_DEPTH_TEST);
	gs->render(shadowMap->m_shader, 0);
	shadowMap->finish();

	ms->render();
	check_gl_error();
	//renderArrow();
	return;
	// Render the shadow buffer on screen
	glClear(GL_DEPTH_BUFFER_BIT);
	check_gl_error();
	glViewport(0, 0, 500, 500);
	check_gl_error();
	quadShader->use();
	check_gl_error();

	glEnable(GL_TEXTURE_2D);
	check_gl_error();
	glActiveTexture(GL_TEXTURE0);
	/*check_gl_error();
	int loc = glGetUniformLocation(quadShader->m_shader_program, "Texture");
	check_gl_error();
	if (loc >= 0)
		glUniform1i(loc, GL_TEXTURE0);
	check_gl_error();*/
	glBindTexture(GL_TEXTURE_2D, shadowMap->depthTexture);
	check_gl_error();
	
	GLuint vao;
	glCreateVertexArrays(1, &vao);
	check_gl_error();
	glBindVertexArray(vao);
	check_gl_error();
	glDisable(GL_CULL_FACE);
	check_gl_error();
	GLfloat vertices[8] = { -1,-1,
							1,-1,
							1,1,
							-1,1};
	GLuint vbo;
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), (GLfloat*)(&vertices), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_QUADS, 0, 4);
	check_gl_error();
	glDisableVertexAttribArray(0);
	check_gl_error();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	check_gl_error();
	glBindVertexArray(0);
	check_gl_error();
	glDeleteBuffers(1, &vbo);
	check_gl_error();
	glDeleteVertexArrays(1, &vao);
	check_gl_error();
}

void Scene::renderArrow()
{
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	glViewport(0, 0, w, h);

	lineShader->use();

	glm::vec3 p1 = { 0.0f, 0.0f, 0.0f };
	glm::vec3 p2 = { 1.0f, 0.2f, 0.0f };

	glm::vec3 v = p2 - p1;
	float tx = v.x;
	v.x = v.y;
	v.y = -tx;
	v = glm::normalize(v);
	omen::floatprec thickness = 0.01f;
	
	glm::vec3 v2[4] = {
	{p1.x + v.x * thickness / 2,p1.y + v.y * thickness / 2,0},
	{p1.x - v.x * thickness / 2,p1.y - v.y * thickness / 2,0},
	{p2.x + v.x * thickness / 2,p2.y + v.y * thickness / 2,0},
	{p2.x - v.x * thickness / 2,p2.y - v.y * thickness / 2,0},
	};
	GLuint vbo2 = 0, vao2 = 0;

	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);

	glGenBuffers(1, &vbo2);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v2), v2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	if( true )
		return;

	
}

void Scene::renderBackground()
{
	renderBuffer->use();
	bgTexture->bind();
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	glViewport(0, 0, w, h);

	bgShaderPass1->use();
	bgShaderPass1->setUniform1f("Blur", blur);

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

	renderBuffer->unuse();
	renderBuffer2->use();
	glBindTexture(GL_TEXTURE_2D, renderBuffer->texture());

	bgShaderPass2->use();
	bgShaderPass2->setUniform1f("Blur", blur);

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	

	ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();
	gs->render();

	

	glBindTexture(GL_TEXTURE_2D, renderBuffer2->texture());

	/*bgShaderPass3->use();
	bgShaderPass3->setUniform1f("Blur", blur);

	glBindVertexArray(vao);
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
	*/
}

void omen::Scene::addEntity(std::unique_ptr<ecs::Entity> entity, GLuint layer)
{
	ecs::Entity* eptr = entity.get();
	entity->setLayer(layer);
	m_entities[layer].push_back(std::move(entity));
	signal_entity_added.notify(eptr);
}

ecs::Entity* Scene::findEntity(const std::string& name)
{
	for(const auto& key : m_entities)
	for (const auto& e : entities(key.first)) {
		if (e->name() == name)
			return e.get();
		ecs::Entity* c = e->findChild(name);
		if (c != nullptr)
			return c;
	}
	return nullptr;
}	