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

	glm::vec2 v[4] = { { -1.0,1.0 },{ -1.0,-1.0 },{ 1.0, 1.0 },{ 1.0,-1.0 } };
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//m_shader = new Shader("shaders/sky.glsl");
	m_shader = new Shader("shaders/spherical_sky.glsl");
	uvTexMap = new Texture("textures/sky_daytime_blue.jpg");
	uvEnvMap = new Texture("textures/hemispherical_006.jpg");

	Engine::instance()->properties()["SpotBrightness"].signal_property_changed.connect(this,[&](std::any* prop) {this->m_spotBrightness = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["RayleighBrightness"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_rayleighBrightness = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["MieBrightness"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_mieBrightness = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["MieDistribution"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_mieDistribution = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["StepCount"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_stepCount = std::any_cast<int>(*prop); });
	Engine::instance()->properties()["SurfaceHeight"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_surfaceHeight = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["RayleighStrength"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_rayleighStrength = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["MieStrength"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_mieStrength = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["ScatterStrength"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_scatterStrength = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["RayleighCollectionPower"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_rayleighCollectionPower = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["MieCollectionPower"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_mieCollectionPower = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["IntensityRed"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_intensityRed = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["IntensityGreen"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_intensityGreen = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["IntensityBlue"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_intensityBlue = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["HExtinctionBias"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_HExtinctionBias = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["EyeExtinctionBias"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_EyeExtinctionBias = std::any_cast<float>(*prop); });
	Engine::instance()->properties()["RayleighFactor"].signal_property_changed.connect(this,[this](std::any* prop) { this->m_rayleighFactor = std::any_cast<float>(*prop); });

	Engine::instance()->properties()["SpotBrightness"] = 480.9f;
	Engine::instance()->properties()["RayleighBrightness"] = 0.542f;
	Engine::instance()->properties()["MieBrightness"] = 0.454f;
	Engine::instance()->properties()["MieDistribution"] = 0.02534f;
	Engine::instance()->properties()["StepCount"] = 2;
	Engine::instance()->properties()["SurfaceHeight"] = 0.3817f;
	Engine::instance()->properties()["RayleighStrength"] = 0.207f;
	Engine::instance()->properties()["RayleighFactor"] = -0.2529f;
	Engine::instance()->properties()["MieStrength"] = 0.1085f;
	Engine::instance()->properties()["ScatterStrength"] = 0.3987f;
	Engine::instance()->properties()["RayleighCollectionPower"] = 0.8642f;
	Engine::instance()->properties()["MieCollectionPower"] = 0.1673f;
	Engine::instance()->properties()["IntensityRed"] = 1.0f;
	Engine::instance()->properties()["IntensityGreen"] = 1.0f;
	Engine::instance()->properties()["IntensityBlue"] = 1.0f;
	Engine::instance()->properties()["Azimuth"] = 1.0f;
	Engine::instance()->properties()["Zenith"] = 0.33f;
	Engine::instance()->properties()["HExtinctionBias"] = 0.35f;
	Engine::instance()->properties()["EyeExtinctionBias"] = 0.00015f;
}


SkyRenderer::~SkyRenderer() {

	delete m_shader;
}

void SkyRenderer::render(Shader* shader) {
	auto start = std::chrono::high_resolution_clock::now();
	omen::Window::_size s = Engine::instance()->window()->size();

	m_shader->use();
	glm::mat4 view = Engine::instance()->camera()->view();
	glm::mat4 proj = Engine::instance()->camera()->projection();
	glm::mat4 ipv = glm::inverse(proj*view);
	glm::mat4 ip = glm::inverse(proj);
	glm::mat4 iv = glm::inverse(view);
		
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glEnable(GL_TEXTURE_2D);
	glClear(GL_DEPTH_BUFFER_BIT);

	/*GLint uvMapLoc = m_shader->getUniformLocation("Texture");
	glUniform1i(uvMapLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	uvTexMap->bind();

	GLint uvEnvMapLoc = m_shader->getUniformLocation("uTexEnv");
	glUniform1i(uvMapLoc, 1);
	glActiveTexture(GL_TEXTURE1);
	uvEnvMap->bind();*/

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

	glm::vec3 Kr = glm::vec3(0.18867780436772762*(0.5 + 0.5*m_intensityRed), 0.4978442963618773*(0.5 + 0.5*m_intensityGreen), 0.6616065586417131*(0.5 + 0.5*m_intensityBlue));
	m_shader->setUniform3fv("Kr", 1, glm::value_ptr(Kr));


	glm::vec3 lpos = std::any_cast<glm::vec3>(Engine::LightPos);
	m_shader->setUniform3fv("LightPos", 1, glm::value_ptr(lpos));

	m_shader->setUniform1i("HasTexture", false);
	int textureLoc = m_shader->getUniformLocation("DiffuseColorMap");
	if (textureLoc >= 0 && uvEnvMap != nullptr && uvEnvMap->id() >= 0) {
		m_shader->setUniform1i("HasDiffuseTexture", true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(textureLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, uvEnvMap->id());
	}
	else
		m_shader->setUniform1i("HasDiffuseTexture", false);



	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	drawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glClear(GL_DEPTH_BUFFER_BIT);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	double ms = diff.count() * 1000.0f;
	//std::cout << "Sky render: " << ms << "ms.\n";
}
