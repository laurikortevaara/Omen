#include "SkyRenderer.h"
#include "component/MeshController.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace omen;
using namespace ecs;

Texture * uvEnvMap = nullptr;
Texture * uvTexMap = nullptr;

SkyRenderer::SkyRenderer(MeshController* mc) :
	MeshRenderer(mc) {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	//m_shader = new Shader("shaders/sky.glsl");
	m_shader = new Shader("shaders/spherical_sky.glsl");
	uvTexMap = new Texture("textures/sky_daytime_blue.jpg");
	uvEnvMap = new Texture("textures/sky_daytime_blue.jpg");

	Engine::instance()->properties["SpotBrightness"].signal_property_changed.connect([&](std::any* prop) { 
	   float miu = std::any_cast<float>(*prop);
		this->m_spotBrightness = miu; 
	});

	Engine::instance()->properties["RayleighBrightness"].signal_property_changed.connect([this](std::any* prop) { this->m_rayleighBrightness = std::any_cast<float>(*prop); });
	Engine::instance()->properties["MieBrightness"].signal_property_changed.connect([this](std::any* prop) { this->m_mieBrightness = std::any_cast<float>(*prop); });
	Engine::instance()->properties["MieDistribution"].signal_property_changed.connect([this](std::any* prop) { this->m_mieDistribution = std::any_cast<float>(*prop); });
	Engine::instance()->properties["StepCount"].signal_property_changed.connect([this](std::any* prop) { this->m_stepCount = std::any_cast<int>(*prop); });
	Engine::instance()->properties["SurfaceHeight"].signal_property_changed.connect([this](std::any* prop) { this->m_surfaceHeight = std::any_cast<float>(*prop); });
	Engine::instance()->properties["RayleighStrength"].signal_property_changed.connect([this](std::any* prop) { this->m_rayleighStrength = std::any_cast<float>(*prop); });
	Engine::instance()->properties["MieStrength"].signal_property_changed.connect([this](std::any* prop) { this->m_mieStrength = std::any_cast<float>(*prop); });
	Engine::instance()->properties["ScatterStrength"].signal_property_changed.connect([this](std::any* prop) { this->m_scatterStrength = std::any_cast<float>(*prop); });
	Engine::instance()->properties["RayleighCollectionPower"].signal_property_changed.connect([this](std::any* prop) { this->m_rayleighCollectionPower = std::any_cast<float>(*prop); });
	Engine::instance()->properties["MieCollectionPower"].signal_property_changed.connect([this](std::any* prop) { this->m_mieCollectionPower = std::any_cast<float>(*prop); });
	Engine::instance()->properties["IntensityRed"].signal_property_changed.connect([this](std::any* prop) { this->m_intensityRed = std::any_cast<float>(*prop); });
	Engine::instance()->properties["IntensityGreen"].signal_property_changed.connect([this](std::any* prop) { this->m_intensityGreen = std::any_cast<float>(*prop); });
	Engine::instance()->properties["IntensityBlue"].signal_property_changed.connect([this](std::any* prop) { this->m_intensityBlue = std::any_cast<float>(*prop); });
	Engine::instance()->properties["HExtinctionBias"].signal_property_changed.connect([this](std::any* prop) { this->m_HExtinctionBias = std::any_cast<float>(*prop); });
	Engine::instance()->properties["EyeExtinctionBias"].signal_property_changed.connect([this](std::any* prop) { this->m_EyeExtinctionBias = std::any_cast<float>(*prop); });
	Engine::instance()->properties["RayleighFactor"].signal_property_changed.connect([this](std::any* prop) { this->m_rayleighFactor = std::any_cast<float>(*prop); });
}


SkyRenderer::~SkyRenderer() {

	delete m_shader;
}

void SkyRenderer::render(Shader* shader) {
	omen::Window::_size s = Engine::instance()->window()->size();

	m_shader->use();
	glm::mat4 view = Engine::instance()->camera()->view();
	glm::mat4 proj = Engine::instance()->camera()->projection();
	glm::mat4 ipv = glm::inverse(proj*view);
	glm::mat4 ip = glm::inverse(proj);
	glm::mat4 iv = glm::inverse(view);
	
	glm::vec2 v[4] = { { -1.0,1.0 },{ -1.0,-1.0 },{ 1.0, 1.0 },{ 1.0,-1.0 } };
	GLuint vbo = 0, vao = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnable(GL_TEXTURE_2D);
	glClear(GL_DEPTH_BUFFER_BIT);

	GLint uvMapLoc = m_shader->getUniformLocation("Texture");
	glUniform1i(uvMapLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	uvTexMap->bind();

	GLint uvEnvMapLoc = m_shader->getUniformLocation("uTexEnv");
	glUniform1i(uvMapLoc, 1);
	glActiveTexture(GL_TEXTURE1);
	uvEnvMap->bind();

	m_shader->setUniformMatrix4fv("invPV", 1, glm::value_ptr(ipv), false);
	m_shader->setUniformMatrix4fv("inv_proj", 1, glm::value_ptr(ip), false);
	m_shader->setUniformMatrix4fv("inv_view_rot", 1, glm::value_ptr(iv), false);
	
	m_shader->setUniform1f("SpotBrightness", m_spotBrightness); 
	m_shader->setUniform1f("RayleighBrightness", m_rayleighBrightness);
	m_shader->setUniform1f("MieBrightness", m_mieBrightness); 
	m_shader->setUniform1f("MieDistribution", m_mieDistribution);
	m_shader->setUniform1i("StepCount", m_stepCount); 
	m_shader->setUniform1f("SurfaceHeight", m_surfaceHeight);
	m_shader->setUniform1f("RayleighStrength", m_rayleighStrength);
	m_shader->setUniform1f("MieStrength", m_mieStrength); 
	m_shader->setUniform1f("ScatterStrength", m_scatterStrength); 
	m_shader->setUniform1f("RayleighCollectionPower", m_rayleighCollectionPower); 
	m_shader->setUniform1f("MieCollectionPower", m_mieCollectionPower); 
	m_shader->setUniform1f("IntensityRed", m_intensityRed); 
	m_shader->setUniform1f("IntensityGreen", m_intensityGreen);
	m_shader->setUniform1f("IntensityBlue", m_intensityBlue); 
	m_shader->setUniform1f("HExtinctionBias", m_HExtinctionBias); 
	m_shader->setUniform1f("EyeExtinctionBias", m_EyeExtinctionBias);
	m_shader->setUniform1f("RayleighFactor", m_rayleighFactor);

	glm::vec3 lpos = std::any_cast<glm::vec3>(Engine::LightPos);
	m_shader->setUniform3fv("LightPos", 1, glm::value_ptr(lpos));


	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glClear(GL_DEPTH_BUFFER_BIT);
}
