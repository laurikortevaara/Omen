//
// Created by Lauri Kortevaara on 08/11/15.
//

#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

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
#include "ui/LinearLayout.h"
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
#include "Octree.h"
#include "Voxel.h"
#include "OctreeRenderer.h"

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
		Engine::instance()->setViewport(0, 0, w, h);

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
		ecs::MeshRenderer* mr = obj->getComponent<ecs::MeshRenderer>();

		std::unique_ptr<Mesh>const & m = mr->meshController()->mesh();
		std::unique_ptr<Octree> octree = std::make_unique<Octree>(m.get());
		/*Engine::instance()->signal_engine_update.connect([file](float time, float delta_time) {
		ecs::GameObject* obj = dynamic_cast<ecs::GameObject*>(Engine::instance()->scene()->findEntity(file));
		obj->tr()->rotate(time, glm::vec3(0.75, 0, 0));
		obj->tr()->rotate(time, glm::vec3(0, 0.34, 0));
		});*/
		std::unique_ptr<ecs::OctreeRenderer> or = std::make_unique<ecs::OctreeRenderer>(std::move(octree));
		obj->addCompnent(std::move(or));
		addEntity(std::move(obj));

	}
	
	return nullptr;
}
void omen::Scene::initialize()
{
	std::unique_ptr<ui::LinearLayout> sliderLayout = std::make_unique<ui::LinearLayout>(nullptr, "SliderLayout", glm::vec2(0), glm::vec2(500, 500), ui::LinearLayout::VERTICAL);

	std::unique_ptr<ui::Slider> slider_spot_brightness = std::make_unique<ui::Slider>(nullptr, "Spot Brightness", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0,1000));
	Engine::instance()->properties["SpotBrightness"] = 1000.0f;
	slider_spot_brightness->setCurrentValue(1000.0f);
	slider_spot_brightness->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["SpotBrightness"] = value;
	});
	sliderLayout->addChild(std::move(slider_spot_brightness));

	std::unique_ptr<ui::Slider> slider_rayleigh_brightness = std::make_unique<ui::Slider>(nullptr, "Rayleigh Brightness", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 10));
	Engine::instance()->properties["RayleighBrightness"] = 1.0f;
	slider_rayleigh_brightness->setCurrentValue(1.0f);
	slider_rayleigh_brightness->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["RayleighBrightness"] = value;
	});
	sliderLayout->addChild(std::move(slider_rayleigh_brightness));

	std::unique_ptr<ui::Slider> slider_mie_brightness = std::make_unique<ui::Slider>(nullptr, "Mie Brightness", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 10));
	Engine::instance()->properties["MieBrightness"] = 0.15f;
	slider_mie_brightness->setCurrentValue(0.15f);
	slider_mie_brightness->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["MieBrightness"] = value;
	});
	sliderLayout->addChild(std::move(slider_mie_brightness));

	std::unique_ptr<ui::Slider> slider_mie_distribution = std::make_unique<ui::Slider>(nullptr, "Mie Distribution", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 0.18));
	Engine::instance()->properties["MieDistribution"] = 0.02f;
	slider_mie_distribution->setCurrentValue(0.02f);
	slider_mie_distribution->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["MieDistribution"] = value;
	});
	sliderLayout->addChild(std::move(slider_mie_distribution));

	std::unique_ptr<ui::Slider> slider_step_count = std::make_unique<ui::Slider>(nullptr, "StepCount", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 500));
	Engine::instance()->properties["StepCount"] = 10;
	slider_step_count->setCurrentValue(10);
	slider_step_count->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["StepCount"] = static_cast<int>(value);
	});
	sliderLayout->addChild(std::move(slider_step_count));

	std::unique_ptr<ui::Slider> slider_surface_height = std::make_unique<ui::Slider>(nullptr, "SurfaceHeight", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 1));
	Engine::instance()->properties["SurfaceHeight"] = 0.5f;
	slider_surface_height->setCurrentValue(0.5f);
	slider_surface_height->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["SurfaceHeight"] = value;
	});
	sliderLayout->addChild(std::move(slider_surface_height));

	std::unique_ptr<ui::Slider> slider_rayleigh_strenght = std::make_unique<ui::Slider>(nullptr, "RayleighStrength", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties["RayleighStrength"] = 2.0f;
	slider_rayleigh_strenght->setCurrentValue(2.0f);
	slider_rayleigh_strenght->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["RayleighStrength"] = value;
	});
	sliderLayout->addChild(std::move(slider_rayleigh_strenght));

	std::unique_ptr<ui::Slider> slider_mie_strength = std::make_unique<ui::Slider>(nullptr, "MieStrength", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties["MieStrength"] = 1.3f;
	slider_mie_strength->setCurrentValue(1.3f);
	slider_mie_strength->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["MieStrength"] = value;
	});
	sliderLayout->addChild(std::move(slider_mie_strength));

	std::unique_ptr<ui::Slider> slider_scatter_strength = std::make_unique<ui::Slider>(nullptr, "ScatterStrength", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties["ScatterStrength"] = 2.0f;
	slider_scatter_strength->setCurrentValue(2.0f);
	slider_scatter_strength->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["ScatterStrength"] = value;
	});
	sliderLayout->addChild(std::move(slider_scatter_strength));

	std::unique_ptr<ui::Slider> slider_rayleigh_collection_power = std::make_unique<ui::Slider>(nullptr, "RayleighColPower", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties["RayleighCollectionPower"] = 0.801f;
	slider_rayleigh_collection_power->setCurrentValue(0.801f);
	slider_rayleigh_collection_power->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["RayleighCollectionPower"] = value;
	});
	sliderLayout->addChild(std::move(slider_rayleigh_collection_power));

	std::unique_ptr<ui::Slider> slider_mie_collection_power = std::make_unique<ui::Slider>(nullptr, "MieColPower", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties["MieCollectionPower"] = 0.56f;
	slider_mie_collection_power->setCurrentValue(0.56f);
	slider_mie_collection_power->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["MieCollectionPower"] = value;
	});
	sliderLayout->addChild(std::move(slider_mie_collection_power));

	std::unique_ptr<ui::Slider> slider_intensity_red = std::make_unique<ui::Slider>(nullptr, "Intens Red", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties["IntensityRed"] = 1.0f;
	slider_intensity_red->setCurrentValue(1.0f);
	slider_intensity_red->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["IntensityRed"] = value;
	});
	sliderLayout->addChild(std::move(slider_intensity_red));

	std::unique_ptr<ui::Slider> slider_intensity_green = std::make_unique<ui::Slider>(nullptr, "Intens Green", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties["IntensityGreen"] = 1.0f;
	slider_intensity_green->setCurrentValue(1.0f);
	slider_intensity_green->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["IntensityGreen"] = value;
	});
	sliderLayout->addChild(std::move(slider_intensity_green));

	std::unique_ptr<ui::Slider> slider_intensity_blue = std::make_unique<ui::Slider>(nullptr, "Intens Blue", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0.001, 2));
	Engine::instance()->properties["IntensityBlue"] = 1.0f;
	slider_intensity_blue->setCurrentValue(1.0f);
	slider_intensity_blue->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["IntensityBlue"] = value;
	});
	sliderLayout->addChild(std::move(slider_intensity_blue));

	std::unique_ptr<ui::Slider> slider_sun_azimuth = std::make_unique<ui::Slider>(nullptr, "Sun Azimuth", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 360));
	Engine::instance()->properties["Azimuth"] = 1.0f;
	slider_sun_azimuth->setCurrentValue(1.0f);
	slider_sun_azimuth->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["Azimuth"] = value;
	});
	sliderLayout->addChild(std::move(slider_sun_azimuth));

	std::unique_ptr<ui::Slider> slider_sun_zenith = std::make_unique<ui::Slider>(nullptr, "Sun Zenith", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 360));
	Engine::instance()->properties["Zenith"] = 1.0f;
	slider_sun_zenith->setCurrentValue(1.0f);
	slider_sun_zenith->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["Zenith"] = value;
	});
	sliderLayout->addChild(std::move(slider_sun_zenith));

	std::unique_ptr<ui::Slider> slider_hbias = std::make_unique<ui::Slider>(nullptr, "HExt Bias", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 1.0f));
	Engine::instance()->properties["HExtinctionBias"] = 0.35f;
	slider_hbias->setCurrentValue(0.35f);
	slider_hbias->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["HExtinctionBias"] = value;
	});
	sliderLayout->addChild(std::move(slider_hbias));

	std::unique_ptr<ui::Slider> slider_ebias = std::make_unique<ui::Slider>(nullptr, "EyeExt Bias", "textures/slider_groove.png", glm::vec2(0, 0), glm::vec2(500, 25), glm::vec2(0, 1.0f));
	Engine::instance()->properties["EyeExtinctionBias"] = 0.15f;
	slider_ebias->setCurrentValue(0.15f);
	slider_ebias->signal_slider_dragged.connect([](ui::Slider* slider, float value) {
		Engine::instance()->properties["EyeExtinctionBias"] = value;
	});
	sliderLayout->addChild(std::move(slider_ebias));

	addEntity(std::move(sliderLayout));
	addEntity(std::move(std::make_unique<Sky>()));
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
	// Render scene on screen
	ms->render();
}

void Scene::renderArrow()
{
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	Engine::instance()->setViewport(0, 0, w, h);

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
	Engine::instance()->setViewport(0, 0, w, h);

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