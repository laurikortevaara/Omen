//
// Created by Lauri Kortevaara on 08/11/15.
//
#include "Scene.h"

#include "Engine.h"
#include "GameObject.h"
#include "GroundGrid.h"
#include "MD3Loader.h"
#include "MeshProvider.h"
#include "Model.h"
#include "MultipassShader.h"
#include "MathUtils.h"
#include "Ocean.h"
#include "Octree.h"
#include "OctreeRenderer.h"
#include "PointLight.h"
#include "ShadowMap.h"
#include "SkyBox.h"
#include "RenderBuffer.h"
#include "Terrain.h"
#include "Texture.h"
#include "ToolView.h"
#include "utils.h"
#include "Voxel.h"
#include "WavefrontLoader.h"

#include "component/JoystickInput.h"
#include "component/KeyboardInput.h"
#include "component/MouseInput.h"
#include "component/MeshRenderer.h"
#include "component/Picker.h"
#include "component/MeshController.h"

#include "system/GraphicsSystem.h"

#include "ui/Button.h"
#include "ui/Slider.h"
#include "ui/LinearLayout.h"
#include "ui/TextView.h"

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

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
float blur = 0;
ShadowMap* shadowMap = nullptr;
ecs::GameObject* lightobj = nullptr;

namespace fs = std::experimental::filesystem;

Scene::Scene() : omen::Object("Scene") {
	
	Engine* e = Engine::instance();

	// Two pass gauss blur for the background
	//quadShader = new Shader("shaders/quad.glsl");
	//lineShader = new Shader("shaders/line_shader.glsl");
	bgShaderPass1 = new Shader("shaders/texture_quad.glsl");
	//bgShaderPass2 = new Shader("shaders/ring_twisterdistance_field_text.glsl");

	//bgShaderPass3 = new Shader("shaders/physical_chromatic_aberration.glsl");
	renderBuffer = new RenderBuffer();
	renderBuffer2 = new RenderBuffer();

	ms = new MultipassShader();
	/*ms->addPass(MultipassShader::RenderPass(bgShaderPass1, [&](MultipassShader* multipass, Shader* shader, unsigned int pass) {
		renderBuffer->use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int textureMapLoc = shader->getUniformLocation("Texture");
		glUniform1i(textureMapLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		bgTexture->bind();
				
		bgShaderPass1->use();
		bgShaderPass1->setUniform1f("Blur", blur);

		glm::vec2 v[4] = { { -1.0,1.0 },{ -1.0,-1.0 },{ 1.0, 1.0 },{ 1.0,-1.0 } };
		uint32_t vbo = 0, vao = 0;

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
		drawArrays(GL_TRIANGLE_STRIP, 0, 4);
		renderBuffer->unuse();
	}));
	*/
	ms->addPass(MultipassShader::RenderPass(bgShaderPass1, [&](MultipassShader* multipass, Shader* shader, unsigned int pass) {
		ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();
		glClear(GL_DEPTH_BUFFER_BIT);
		gs->render(nullptr, 0);
		if (true)
		{
			//glClear(GL_DEPTH_BUFFER_BIT);
			gs->render(nullptr, 1);
			//glClear(GL_DEPTH_BUFFER_BIT);
			//gs->render(nullptr, 2);
			//gs->render(nullptr, 3);
		}
	}));
	

	
	// Random background image
	std::vector<std::string> bgFiles;
	for (auto& filename : fs::directory_iterator("textures/")) {
		fs::directory_entry e(filename);
		std::ostringstream os;
		os << e;
		bgFiles.push_back(os.str());
	}

	JoystickInput* ji = (JoystickInput*)e->findComponent<JoystickInput>();
	if (ji != nullptr) {
		ji->joystick_button_pressed.connect(this, [&](Joystick *joystick) {

		});
	}

	MouseInput *mi = Engine::instance()->findComponent<MouseInput>();
	if (mi != nullptr) {
		mi->signal_mouse_scrolled.connect(this, [&](omen::floatprec x, omen::floatprec y) {
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

	std::unique_ptr<omen::ecs::GameObject> gameObject = provider->loadObject(filename);
	int i = 0;

	/*ecs::MeshRenderer* mr = gameObject->getComponent<ecs::MeshRenderer>();

	std::unique_ptr<Mesh>const & m = mr->meshController()->mesh();
	std::unique_ptr<Octree> octree = std::make_unique<Octree>(m.get());
	/*Engine::instance()->signal_engine_update.connect(this,[file](float time, float delta_time) {
	ecs::GameObject* obj = dynamic_cast<ecs::GameObject*>(Engine::instance()->scene()->findEntity(file));
	obj->tr()->rotate(time, glm::vec3(0.75, 0, 0));
	obj->tr()->rotate(time, glm::vec3(0, 0.34, 0));
	});*/
		
	//std::unique_ptr<ecs::OctreeRenderer> or = std::make_unique<ecs::OctreeRenderer>(std::move(octree));
	//obj->addCompnent(std::move(or));
	addEntity(std::move(gameObject));

	if (lights().empty())
	{
		std::unique_ptr<Light> light = std::make_unique<DirectionalLight>(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), 1.0f);
		Light* l = light.get();
		std::unique_ptr<omen::ecs::GameObject> lightBulb = provider->loadObject("models/light_bulb.fbx");
		lightBulb->tr()->setPos(light->tr().pos());
		lightBulb->updateBounds();
		lightBulb->tr()->rotate(90.0, glm::vec3(1, 0, 0));
		lightBulb->setName(light->name());
		lightBulb->tr()->signal_transform_changed.connect(this,[l](omen::ecs::Entity* e, omen::Transform* t) {
				glm::vec3 p = t->pos();
				std::cout << "LPos: " << p.x << ", " << p.y << ", " << p.z << '\n';
				l->setPosition(t->pos());
			});
		addEntity(std::move(lightBulb));
		lights().push_back(std::move(light));
	}
	else
	for (auto& light : lights()) {
		std::unique_ptr<omen::ecs::GameObject> lightBulb = provider->loadObject("models/light_bulb.fbx");
		Light* l = light.get();
		lightBulb->tr()->setPos(light->tr().pos());
		lightBulb->updateBounds();
		lightBulb->tr()->rotate(90.0, glm::vec3(1, 0, 0));
		lightBulb->setName(light->name());
		lightBulb->tr()->signal_transform_changed.connect(this,[l](omen::ecs::Entity* e, omen::Transform* t) {
			glm::vec3 p = t->pos();
			std::cout << "LPos: " << p.x << ", " << p.y << ", " << p.z << '\n';
			l->setPosition(t->pos()); 
		});
		addEntity(std::move(lightBulb));
		
	}
	
	return nullptr;
}
void omen::Scene::initialize()
{
	return;
	shadowMap = new ShadowMap();
	shadowMap->init();

	std::unique_ptr<ui::TextView> t = std::make_unique<ui::TextView>(nullptr, "textview", glm::vec2(100, 100), glm::vec2(1024, 1024));
	t->setText(L"Hello world");
	addEntity(std::move(t));

	std::unique_ptr<ui::TextView> t2 = std::make_unique<ui::TextView>(nullptr, "textview_selected", glm::vec2(10, Engine::instance()->window()->height()-10), glm::vec2(1024, 1024));
	t2->setText(L"Hello world");
	addEntity(std::move(t2));

	Engine::instance()->findComponent<omen::Picker>()->signal_object_picked.connect(this,[&](omen::ecs::Entity* e1, glm::vec3 pos) {
		ui::TextView* t = dynamic_cast<ui::TextView*>(findEntity("textview_selected"));
		std::string selected;
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wostringstream is;
		is << L"Selected object: " << (e1 != nullptr ? converter.from_bytes(e1->name()) : L"Nothing");
		t->setText(is.str());
	});

	Engine::instance()->signal_engine_update.connect(this,[&](float time, float delta_time) {
		return;
		ui::TextView* t = dynamic_cast<ui::TextView*>(findEntity("textview_selected"));
		std::string selected;
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wostringstream is;
		
		glm::ivec4 viewport;
		glGetIntegerv(GL_VIEWPORT, (int *)&viewport);

		MouseInput *mi = Engine::instance()->findComponent<MouseInput>();
		float mouseX = mi->cursorPos().x / (Engine::instance()->window()->width()  * 0.5f) - 1;
		float mouseY = mi->cursorPos().x / (Engine::instance()->window()->width() * 0.5f) - 1;

		float mouseX2 = mi->cursorPos().x / (viewport[2]  * 0.5f) - 1;
		float mouseY2 = mi->cursorPos().x / (viewport[3] * 0.5f) - 1;
		omen::ecs::Entity* e1 = Picker::CurrentlySelected;
		is << L"Mouse Pos: " << L"Selected object: " << (e1 != nullptr ? converter.from_bytes(e1->name()) : L"Nothing") << mi->cursorPos().x << ", " << mi->cursorPos().y << "(" << mouseX << ", " << mouseY << ")" << ", " << "(" << mouseX2 << ", " << mouseY2 << ")";
		t->setText(is.str());
	});


	//addEntity(std::move(std::make_unique<SkyBox>()));
	//addEntity(std::move(std::make_unique<Ocean>()));
	std::unique_ptr<ui::LinearLayout> sliderLayout = std::make_unique<ui::LinearLayout>(nullptr, "SliderLayout", glm::vec2(0), glm::vec2(500, 500), ui::LinearLayout::VERTICAL);

	std::unique_ptr<ui::Slider> slider_spot_brightness = std::make_unique<ui::Slider>(nullptr, "Time", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 10));
	Engine::instance()->properties()["Time"] = 1.0f;
	slider_spot_brightness->setValue(1);
	slider_spot_brightness->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["Time"] = value;
	});
	sliderLayout->addChild(std::move(slider_spot_brightness));

	std::unique_ptr<ui::Slider> slider_A = std::make_unique<ui::Slider>(nullptr, "OceanLen", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.00000001f, 10000), ui::Slider::eInCubic);
	Engine::instance()->properties()["OceanLen"] = 128.0f;
	slider_A->setValue(128);
	slider_A->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["OceanLen"] = value;
	});
	sliderLayout->addChild(std::move(slider_A));

	std::unique_ptr<ui::Slider> slider_D = std::make_unique<ui::Slider>(nullptr, "Damping", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.00001, 1));
	Engine::instance()->properties()["Damping"] = 0.000001f;
	slider_D->setValue(0.000001f);
	slider_D->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["Damping"] = value;
	});
	sliderLayout->addChild(std::move(slider_D));

	std::unique_ptr<ui::Slider> slider_G = std::make_unique<ui::Slider>(nullptr, "Gravity", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.0000000001, 100));
	Engine::instance()->properties()["Gravity"] = 1.f;
	slider_G->setValue(1);
	slider_G->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["Gravity"] = value;
	});
	sliderLayout->addChild(std::move(slider_G));

	std::unique_ptr<ui::Slider> slider_WD = std::make_unique<ui::Slider>(nullptr, "WindDir", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(-M_PI, M_PI));
	Engine::instance()->properties()["WindDir"] = 0.0f;
	slider_WD->setValue(0);
	slider_WD->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["WindDir"] = value;
	});
	sliderLayout->addChild(std::move(slider_WD));

	std::unique_ptr<ui::Slider> slider_WP = std::make_unique<ui::Slider>(nullptr, "WindPower", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 1e3));
	Engine::instance()->properties()["WindPower"] = 32.0f;
	slider_WP->setValue(32);
	slider_WP->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["WindPower"] = value;
	});
	sliderLayout->addChild(std::move(slider_WP));

	int imaxTessLevel;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &imaxTessLevel);
	int maxTessLevel = imaxTessLevel;

	std::unique_ptr<ui::Slider> TessellationLevelInner1 = std::make_unique<ui::Slider>(nullptr, "TessellationLevelInner1", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(1, maxTessLevel));
	Engine::instance()->properties()["TessellationLevelOuter1"] = maxTessLevel / 2.0f;
	TessellationLevelInner1->setValue(maxTessLevel / 2.0f);
	TessellationLevelInner1->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["TessellationLevelInner1"] = value;
	});
	sliderLayout->addChild(std::move(TessellationLevelInner1));

	std::unique_ptr<ui::Slider> TessellationLevelInner2 = std::make_unique<ui::Slider>(nullptr, "TessellationLevelInner2", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(1, maxTessLevel));
	Engine::instance()->properties()["TessellationLevelOuter4"] = maxTessLevel / 2.0f;
	TessellationLevelInner2->setValue(maxTessLevel / 2.0f);
	TessellationLevelInner2->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["TessellationLevelInner2"] = value;
	});
	sliderLayout->addChild(std::move(TessellationLevelInner2));

	std::unique_ptr<ui::Slider> TessellationLevelOuter1 = std::make_unique<ui::Slider>(nullptr, "TessellationLevelOuter1", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(1, maxTessLevel));
	Engine::instance()->properties()["TessellationLevelOuter1"] = maxTessLevel / 2.0f;
	TessellationLevelOuter1->setValue(maxTessLevel / 2.0f);
	TessellationLevelOuter1->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["TessellationLevelOuter1"] = value;
	});
	sliderLayout->addChild(std::move(TessellationLevelOuter1));

	std::unique_ptr<ui::Slider> TessellationLevelOuter2 = std::make_unique<ui::Slider>(nullptr, "TessellationLevelOuter2", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(1, maxTessLevel));
	Engine::instance()->properties()["TessellationLevelOuter2"] = maxTessLevel / 2.0f;
	TessellationLevelOuter2->setValue(maxTessLevel / 2.0f);
	TessellationLevelOuter2->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["TessellationLevelOuter2"] = value;
	});
	sliderLayout->addChild(std::move(TessellationLevelOuter2));

	std::unique_ptr<ui::Slider> TessellationLevelOuter3 = std::make_unique<ui::Slider>(nullptr, "TessellationLevelOuter3", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(1, maxTessLevel));
	Engine::instance()->properties()["TessellationLevelOuter3"] = maxTessLevel / 2.0f;
	TessellationLevelOuter3->setValue(maxTessLevel / 2.0f);
	TessellationLevelOuter3->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["TessellationLevelOuter3"] = value;
	});
	sliderLayout->addChild(std::move(TessellationLevelOuter3));

	std::unique_ptr<ui::Slider> TessellationLevelOuter4 = std::make_unique<ui::Slider>(nullptr, "TessellationLevelOuter4", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(1, maxTessLevel));
	Engine::instance()->properties()["TessellationLevelOuter4"] = maxTessLevel / 2.0f;
	TessellationLevelOuter4->setValue(maxTessLevel / 2.0f);
	TessellationLevelOuter4->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["TessellationLevelOuter4"] = value;
	});
	sliderLayout->addChild(std::move(TessellationLevelOuter4));

	std::unique_ptr<ui::Slider> fov = std::make_unique<ui::Slider>(nullptr, "fov", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(1.0, 179));
	Engine::instance()->properties()["FOV"] = 90.0f;
	fov->setValue(90);
	fov->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["FOV"] = value;
		std::wostringstream wis;
		wis << L"FOV:" << std::setprecision(3) << value << L", L:" << Engine::instance()->camera()->focalLength() << L"mm.";
		slider->setLabel(wis.str());
	});
	sliderLayout->addChild(std::move(fov));

	std::unique_ptr<ui::Slider> zNear = std::make_unique<ui::Slider>(nullptr, "near", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 10000));
	Engine::instance()->properties()["zNear"] = 1.0f;
	zNear->setValue(1);
	zNear->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["zNear"] = value;
	});
	sliderLayout->addChild(std::move(zNear));

	std::unique_ptr<ui::Slider> zFar = std::make_unique<ui::Slider>(nullptr, "far", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(1, 10000));
	Engine::instance()->properties()["zFar"] = 100.0f;
	zFar->setValue(100.0f);
	zFar->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["zFar"] = value;
	});
	sliderLayout->addChild(std::move(zFar));

	std::unique_ptr<ui::Slider> perlinSize = std::make_unique<ui::Slider>(nullptr, "PerlinSize", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.000000001, 1000));
	Engine::instance()->properties()["PerlinSize"] = 1.0f;
	perlinSize->setValue(1.0f);
	perlinSize->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["PerlinSize"] = value;
	});
	sliderLayout->addChild(std::move(perlinSize));

	std::unique_ptr<ui::Slider> PerlinScale = std::make_unique<ui::Slider>(nullptr, "PerlinScale", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.000000001, 1000));
	Engine::instance()->properties()["PerlinScale"] = 1.0f;
	PerlinScale->setValue(1.0f);
	PerlinScale->signal_slider_dragged.connect(this, [](ui::Slider* slider, float value) {
		Engine::instance()->properties()["PerlinScale"] = value;
	});
	sliderLayout->addChild(std::move(PerlinScale));
	

	//addEntity(std::move(sliderLayout));
	//
	/*std::unique_ptr<omen::ecs::GameObject> obj = std::make_unique<omen::ecs::GameObject>("Cube");
	obj->setLayer(0);

	std::unique_ptr<omen::ecs::MeshController> mc = std::make_unique<omen::ecs::MeshController>();
	mc->setMesh(std::move(MeshProvider::createCube()));
	std::unique_ptr<omen::ecs::MeshRenderer> mr = std::make_unique<omen::ecs::MeshRenderer>(mc.get());
	obj->addCompnent(std::move(mr));
	obj->addCompnent(std::move(mc));
	addEntity(std::move(obj));
	*/
	//
	//return;
	//addEntity(std::move(std::make_unique<GroundGrid>()));
	//std::unique_ptr<ui::ImageView> imgView = std::make_unique<ui::ImageView>(nullptr, "Image", "textures/panel.png", glm::vec2(320, 0), glm::vec2(400, 400));
	//addEntity(std::move(imgView));

	/*std::unique_ptr<ui::LinearLayout> sliderLayout = std::make_unique<ui::LinearLayout>(nullptr, "SliderLayout", glm::vec2(0), glm::vec2(500, 500), ui::LinearLayout::VERTICAL);

	std::unique_ptr<ui::Slider> slider_spot_brightness = std::make_unique<ui::Slider>(nullptr, "Spot Brightness", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0,1000));
	Engine::instance()->properties()["SpotBrightness"] = 480.9f;
	slider_spot_brightness->setValue(480.9f);
	slider_spot_brightness->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["SpotBrightness"] = value;
	});
	sliderLayout->addChild(std::move(slider_spot_brightness));
	
	std::unique_ptr<ui::Slider> slider_rayleigh_brightness = std::make_unique<ui::Slider>(nullptr, "Rayleigh Brightness", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 10));
	Engine::instance()->properties()["RayleighBrightness"] = 5.42f;
	slider_rayleigh_brightness->setValue(5.42f);
	slider_rayleigh_brightness->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["RayleighBrightness"] = value;
	});
	sliderLayout->addChild(std::move(slider_rayleigh_brightness));

	std::unique_ptr<ui::Slider> slider_mie_brightness = std::make_unique<ui::Slider>(nullptr, "Mie Brightness", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 10));
	Engine::instance()->properties()["MieBrightness"] = 4.454f;
	slider_mie_brightness->setValue(5.454f);
	slider_mie_brightness->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["MieBrightness"] = value;
	});
	sliderLayout->addChild(std::move(slider_mie_brightness));
	/*
	std::unique_ptr<ui::Slider> slider_mie_distribution = std::make_unique<ui::Slider>(nullptr, "Mie Distribution", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 0.18));
	Engine::instance()->properties()["MieDistribution"] = 0.02534f;
	slider_mie_distribution->setValue(0.02534f);
	slider_mie_distribution->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["MieDistribution"] = value;
	});
	sliderLayout->addChild(std::move(slider_mie_distribution));

	std::unique_ptr<ui::Slider> slider_step_count = std::make_unique<ui::Slider>(nullptr, "StepCount", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 500));
	Engine::instance()->properties()["StepCount"] = 10;
	slider_step_count->setValue(10);
	slider_step_count->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["StepCount"] = static_cast<int>(value);
	});
	sliderLayout->addChild(std::move(slider_step_count));

	std::unique_ptr<ui::Slider> slider_surface_height = std::make_unique<ui::Slider>(nullptr, "SurfaceHeight", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 1));
	Engine::instance()->properties()["SurfaceHeight"] = 0.03817f;
	slider_surface_height->setValue(0.03817f);
	slider_surface_height->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["SurfaceHeight"] = 0.9f+0.1f*value;
	});
	sliderLayout->addChild(std::move(slider_surface_height));

	std::unique_ptr<ui::Slider> slider_rayleigh_strenght = std::make_unique<ui::Slider>(nullptr, "RayleighStrength", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties()["RayleighStrength"] = 0.207f;
	slider_rayleigh_strenght->setValue(0.207f);
	slider_rayleigh_strenght->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["RayleighStrength"] = value;
	});
	sliderLayout->addChild(std::move(slider_rayleigh_strenght));

	std::unique_ptr<ui::Slider> slider_rayleighfactor = std::make_unique<ui::Slider>(nullptr, "RayleighFactor", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(-1.0, 1));
	Engine::instance()->properties()["RayleighFactor"] = -0.2529f;
	slider_rayleighfactor->setValue(-0.2529f);
	slider_rayleighfactor->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["RayleighFactor"] = value;
	});
	sliderLayout->addChild(std::move(slider_rayleighfactor));

	std::unique_ptr<ui::Slider> slider_mie_strength = std::make_unique<ui::Slider>(nullptr, "MieStrength", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties()["MieStrength"] = 0.1085f;
	slider_mie_strength->setValue(0.1085f);
	slider_mie_strength->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["MieStrength"] = value;
	});
	sliderLayout->addChild(std::move(slider_mie_strength));

	std::unique_ptr<ui::Slider> slider_scatter_strength = std::make_unique<ui::Slider>(nullptr, "ScatterStrength", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties()["ScatterStrength"] = 0.3987f;
	slider_scatter_strength->setValue(0.3987f);
	slider_scatter_strength->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["ScatterStrength"] = value;
	});
	sliderLayout->addChild(std::move(slider_scatter_strength));

	std::unique_ptr<ui::Slider> slider_rayleigh_collection_power = std::make_unique<ui::Slider>(nullptr, "RayleighColPower", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties()["RayleighCollectionPower"] = 0.8642f;
	slider_rayleigh_collection_power->setValue(0.8642f);
	slider_rayleigh_collection_power->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["RayleighCollectionPower"] = value;
	});
	sliderLayout->addChild(std::move(slider_rayleigh_collection_power));

	std::unique_ptr<ui::Slider> slider_mie_collection_power = std::make_unique<ui::Slider>(nullptr, "MieColPower", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties()["MieCollectionPower"] = 1.673f;
	slider_mie_collection_power->setValue(1.673f);
	slider_mie_collection_power->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["MieCollectionPower"] = value;
	});
	sliderLayout->addChild(std::move(slider_mie_collection_power));

	std::unique_ptr<ui::Slider> slider_intensity_red = std::make_unique<ui::Slider>(nullptr, "Intens Red", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties()["IntensityRed"] = 0.681f;
	slider_intensity_red->setValue(0.681f);
	slider_intensity_red->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["IntensityRed"] = value;
	});
	sliderLayout->addChild(std::move(slider_intensity_red));

	std::unique_ptr<ui::Slider> slider_intensity_green = std::make_unique<ui::Slider>(nullptr, "Intens Green", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties()["IntensityGreen"] = 1;
	slider_intensity_green->setValue(1);
	slider_intensity_green->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["IntensityGreen"] = value;
	});
	sliderLayout->addChild(std::move(slider_intensity_green));

	std::unique_ptr<ui::Slider> slider_intensity_blue = std::make_unique<ui::Slider>(nullptr, "Intens Blue", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties()["IntensityBlue"] = 0.7573f;
	slider_intensity_blue->setValue(0.7573f);
	slider_intensity_blue->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["IntensityBlue"] = value;
	});
	sliderLayout->addChild(std::move(slider_intensity_blue));

	std::unique_ptr<ui::Slider> slider_sun_azimuth = std::make_unique<ui::Slider>(nullptr, "Sun Azimuth", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(-180, 180));
	Engine::instance()->properties()["Azimuth"] = 1;
	slider_sun_azimuth->setValue(1);
	slider_sun_azimuth->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["Azimuth"] = value;
	});
	sliderLayout->addChild(std::move(slider_sun_azimuth));

	std::unique_ptr<ui::Slider> slider_sun_zenith = std::make_unique<ui::Slider>(nullptr, "Sun Zenith", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 180));
	Engine::instance()->properties()["Zenith"] = 1;
	slider_sun_zenith->setValue(1);
	slider_sun_zenith->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["Zenith"] = value;
	});
	sliderLayout->addChild(std::move(slider_sun_zenith));

	std::unique_ptr<ui::Slider> slider_hbias = std::make_unique<ui::Slider>(nullptr, "HExt Bias", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 1));
	Engine::instance()->properties()["HExtinctionBias"] = 0.35f;
	slider_hbias->setValue(0.35f);
	slider_hbias->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["HExtinctionBias"] = value;
	});
	sliderLayout->addChild(std::move(slider_hbias));

	std::unique_ptr<ui::Slider> slider_ebias = std::make_unique<ui::Slider>(nullptr, "EyeExt Bias", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 1));
	Engine::instance()->properties()["EyeExtinctionBias"] = 0.00015f;
	slider_ebias->setValue(0.0015f);
	slider_ebias->signal_slider_dragged.connect(this,[](ui::Slider* slider, float value) {
		Engine::instance()->properties()["EyeExtinctionBias"] = value;
	});
	sliderLayout->addChild(std::move(slider_ebias));
	*/
	//addEntity(std::move(sliderLayout));
	/*std::unique_ptr<omen::ui::LinearLayout> mainLayout = std::make_unique<omen::ui::LinearLayout>(nullptr, "SliderLayout", glm::vec2(0), glm::vec2(500, 500), omen::ui::LinearLayout::VERTICAL);
	for (int j = 0; j < 10; ++j)
	{
		std::unique_ptr<omen::ui::LinearLayout> sliderLayout = std::make_unique<omen::ui::LinearLayout>(nullptr, "SliderLayout", glm::vec2(0), glm::vec2(500,40), omen::ui::LinearLayout::HORIZONTAL);
		for (int i = 0; i < 10; ++i)
		{
			std::unique_ptr<omen::ui::ImageView> imgView = std::make_unique<omen::ui::ImageView>(nullptr, "Image", i % 2 ? "textures/slider_knot.png" : "textures/custom_uv_diag.png", glm::vec2( 0,0 ), glm::vec2( 12, 12 ));
			sliderLayout->addChild(std::move(imgView));
		}
		mainLayout->addChild(std::move(sliderLayout));
	}
	addEntity(std::move(mainLayout));*/

}

void Scene::render(const glm::mat4 &viewProjection, const glm::mat4 &view) 
{
	ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();

	// Render shadowmap
	/*shadowMap->render();
	glEnable(GL_DEPTH_TEST);
	gs->render(shadowMap->m_shader, 0);
	shadowMap->finish();
	*/
	// Render scene on screen
	ms->render();

	return;

	// Render the depth texture
	quadShader->use();

	glm::vec2 v[4] = { { -0.1,0.1 },{ -0.1,-0.1 },{ 0.1, 0.1 },{ 0.1,-0.1 } };
	uint32_t vbo = 0, vao = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	int loc = glGetUniformLocation(quadShader->m_shader_program, "Texture");
	if (loc >= 0)
		glUniform1i(loc, GL_TEXTURE0);

	uint32_t texId = shadowMap->depthTexture;
	glBindTexture(GL_TEXTURE_2D, texId);

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	drawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	// Render the camera frustum
	/*lineShader->use();

	float h = Engine::instance()->window()->height();
	float w = Engine::instance()->window()->width();
	float fov = Engine::instance()->camera()->fov()*2.0;
	float ar = Engine::instance()->window()->height() / (float)Engine::instance()->window()->width();

	float tanHalfHFOV = tanf(glm::radians(fov / 2.0f));
	float tanHalfVFOV = tanf(glm::radians((fov * ar) / 2.0f));

	int NUM_CASCADES = 1;
	const int NUM_FRUSTUM_CORNERS = 8;
	float cascadeEnd[2] = { Engine::instance()->camera()->zNear(), Engine::instance()->camera()->zFar() };

	float xn = cascadeEnd[0]	 * tanHalfHFOV;
	float xf = cascadeEnd[0 + 1] * tanHalfHFOV;
	float yn = cascadeEnd[0]	 * tanHalfVFOV;
	float yf = cascadeEnd[0 + 1] * tanHalfVFOV;

	glm::vec4 frustumCorners[NUM_FRUSTUM_CORNERS] = {
		// near face
		glm::vec4(xn, yn, cascadeEnd[0], 1.0),
		glm::vec4(-xn, yn, cascadeEnd[0], 1.0),
		glm::vec4(xn, -yn, cascadeEnd[0], 1.0),
		glm::vec4(-xn, -yn, cascadeEnd[0], 1.0),

		// far face
		glm::vec4(xf, yf, cascadeEnd[0 + 1], 1.0),
		glm::vec4(-xf, yf, cascadeEnd[0 + 1], 1.0),
		glm::vec4(xf, -yf, cascadeEnd[0 + 1], 1.0),
		glm::vec4(-xf, -yf, cascadeEnd[0 + 1], 1.0)
	};

	uint32_t vbo = 0, vao = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(frustumCorners), frustumCorners, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	int loc = glGetUniformLocation(quadShader->m_shader_program, "Texture");
	if (loc >= 0)
		glUniform1i(loc, GL_TEXTURE0);

	uint32_t texId = shadowMap->depthTexture;
	glBindTexture(GL_TEXTURE_2D, texId);

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	drawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);*/
}

void Scene::renderArrow()
{
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	Engine::instance()->setViewport(0, 0, w, h);

	lineShader->use();

	glm::vec3 p1 = { 0, 0, 0 };
	glm::vec3 p2 = { 1, 0.2f, 0 };

	glm::vec3 v = p2 - p1;
	float tx = v.x;
	v.x = v.y;
	v.y = -tx;
	v = glm::normalize(v);
	omen::floatprec thickness = 0.01f;
	
	glm::vec3 v2[4] = {
	{p1.x + v.x * thickness / 2.0f,p1.y + v.y * thickness / 2.0f,0},
	{p1.x - v.x * thickness / 2.0f,p1.y - v.y * thickness / 2.0f,0},
	{p2.x + v.x * thickness / 2.0f,p2.y + v.y * thickness / 2.0f,0},
	{p2.x - v.x * thickness / 2.0f,p2.y - v.y * thickness / 2.0f,0},
	};
	uint32_t vbo2 = 0, vao2 = 0;

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
	drawArrays(GL_TRIANGLE_STRIP, 0, 4);
	if( true )
		return;

	
}

void omen::Scene::addEntity(std::unique_ptr<ecs::Entity> entity, uint32_t layer)
{
	ecs::Entity* eptr = entity.get();
	if(entity->layer()==-1)
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