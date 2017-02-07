#include "SkyRenderer.h"
#include "component/MeshController.h"

using namespace omen;
using namespace ecs;

SkyRenderer::SkyRenderer(MeshController* mc) :
	MeshRenderer(mc) {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	m_shader = new Shader("shaders/sky.glsl");

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	std::vector<glm::vec3> vertices = {
		{ -1,-1,0 },{ 1,-1,0 },{ -1,1,0 },{ 1,1,0 }
	};
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
		0/*stride*/, 0/*offset*/);

	m_indices = { 0,1,2,3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(GLshort), m_indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}


SkyRenderer::~SkyRenderer() {
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);
	delete m_shader;
}

void SkyRenderer::render(Shader* shader) {
	omen::Window::_size s = Engine::instance()->window()->size();

	m_shader->use();
	m_shader->setUniformMatrix4fv("ModelViewProjection", 1, &(Engine::instance()->camera()->viewProjection())[0][0], false);
	m_shader->setUniformMatrix4fv("ViewMatrix", 1, &glm::inverse(Engine::instance()->camera()->view())[0][0], false);
	glm::vec4 viewport = { 0,0,s.width, s.height };
	//m_shader->setUniform4fv("viewport", 1, glm::value_ptr(viewport));
	m_shader->setUniformMatrix4fv("inv_proj", 1, &glm::inverse(Engine::instance()->camera()->projection())[0][0], false);
	glm::mat3x3 inv_view_rot = glm::mat3x3(Engine::instance()->camera()->view());
	m_shader->setUniformMatrix3fv("inv_view_rot", 1, &glm::inverse(inv_view_rot)[0][0], false);
	glm::vec3 Kr(0.18867780436772762, 0.4978442963618773, 0.6616065586417131);
	m_shader->setUniform3fv("Kr", 1, glm::value_ptr(Kr));
	m_shader->setUniform1f("viewport", 1024.0f);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/, 0/*stride*/, 0/*offset*/);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
}