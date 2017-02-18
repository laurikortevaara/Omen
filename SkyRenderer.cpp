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

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
