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
	m_shader->setUniform1f("SpotBrightness", std::any_cast<float>(Engine::instance()->properties["SpotBrightness"]));
	m_shader->setUniform1f("RayleighBrightness", std::any_cast<float>(Engine::instance()->properties["RayleighBrightness"]));
	m_shader->setUniform1f("MieBrightness", std::any_cast<float>(Engine::instance()->properties["MieBrightness"]));
	m_shader->setUniform1f("MieDistribution", std::any_cast<float>(Engine::instance()->properties["MieDistribution"]));
	m_shader->setUniform1i("StepCount", std::any_cast<int>(Engine::instance()->properties["StepCount"]));
	m_shader->setUniform1f("SurfaceHeight", std::any_cast<float>(Engine::instance()->properties["SurfaceHeight"]));
	m_shader->setUniform1f("RayleighStrength", std::any_cast<float>(Engine::instance()->properties["RayleighStrength"]));
	m_shader->setUniform1f("MieStrength", std::any_cast<float>(Engine::instance()->properties["MieStrength"]));
	m_shader->setUniform1f("ScatterStrength", std::any_cast<float>(Engine::instance()->properties["ScatterStrength"]));
	m_shader->setUniform1f("RayleighCollectionPower", std::any_cast<float>(Engine::instance()->properties["RayleighCollectionPower"]));
	m_shader->setUniform1f("MieCollectionPower", std::any_cast<float>(Engine::instance()->properties["MieCollectionPower"]));
	m_shader->setUniform1f("IntensityRed", std::any_cast<float>(Engine::instance()->properties["IntensityRed"]));
	m_shader->setUniform1f("IntensityGreen", std::any_cast<float>(Engine::instance()->properties["IntensityGreen"]));
	m_shader->setUniform1f("IntensityBlue", std::any_cast<float>(Engine::instance()->properties["IntensityBlue"]));
	m_shader->setUniform1f("HExtinctionBias", std::any_cast<float>(Engine::instance()->properties["HExtinctionBias"]));
	m_shader->setUniform1f("EyeExtinctionBias", std::any_cast<float>(Engine::instance()->properties["EyeExtinctionBias"]));
	glm::vec3 lpos = std::any_cast<glm::vec3>(Engine::LightPos);
	m_shader->setUniform3fv("LightPos", 1, glm::value_ptr(lpos));


	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glClear(GL_DEPTH_BUFFER_BIT);
}
