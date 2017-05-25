#include "SkyBoxRenderer.h"
#include "component/MeshController.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace omen;
using namespace ecs;

static GLuint textureID;

Texture * SkyBoxRenderer::uvTexMap = nullptr;

SkyBoxRenderer::SkyBoxRenderer(MeshController* mc) :
	MeshRenderer(mc) {
	
	m_shader = new Shader("shaders/skybox.glsl");
	//uvTexMap = new Texture("textures/skyboxsun25degtest.png", GL_TEXTURE_CUBE_MAP);
	uvTexMap = new Texture("textures/skyboxsun25degtest.png", GL_TEXTURE_CUBE_MAP);
	

	Engine::instance()->window()->signal_file_dropped.connect(this, [this](std::vector<std::string> files) {
		if(files.front().find(".jpg") != std::string::npos || files.front().find(".png") != std::string::npos)
			uvTexMap = new Texture(files.front(), GL_TEXTURE_CUBE_MAP);
	});
}


SkyBoxRenderer::~SkyBoxRenderer() {

	delete m_shader;
}

void SkyBoxRenderer::render(Shader* shader) {
	auto start = std::chrono::high_resolution_clock::now();
	omen::Window::_size s = Engine::instance()->window()->size();

	m_shader->use();
	glm::mat4 view = Engine::instance()->camera()->projection()*glm::mat4(glm::mat3(Engine::instance()->camera()->view()))*glm::scale(glm::mat4(), glm::vec3(100, 100, 100));
	glm::mat4 proj = Engine::instance()->camera()->projection();
	glm::mat4 ipv = glm::inverse(proj*view);
	glm::mat4 ip = glm::inverse(proj);
	glm::mat4 iv = glm::inverse(view);
	
	m_shader->setUniformMatrix4fv("ViewMatrix", 1, glm::value_ptr(view), false);
	m_shader->setUniformMatrix4fv("invPV", 1, glm::value_ptr(ipv), false);
	m_shader->setUniformMatrix4fv("inv_proj", 1, glm::value_ptr(ip), false);
	m_shader->setUniformMatrix4fv("inv_view_rot", 1, glm::value_ptr(iv), false);

	m_shader->setUniform1i("HasTexture", false);
	int textureLoc = m_shader->getUniformLocation("DiffuseColorMap");
	if (textureLoc >= 0 && uvTexMap != nullptr && uvTexMap->id() >= 0) {
		m_shader->setUniform1i("HasDiffuseTexture", true);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(textureLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, uvTexMap->id());
	}
	else
		m_shader->setUniform1i("HasDiffuseTexture", false);

	glDepthMask(GL_FALSE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(VERTEX_ATTRIB_POS);
	storePolygonMode();
	polygonModeWireframe();
	drawElementsInstanced(GL_TRIANGLES, m_indexBufferSize, GL_UNSIGNED_INT, (void*)0, 1);
	restorePolygonMode();

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	double ms = diff.count() * 1000.0f;
	//std::cout << "SkyBox render: " << ms << "ms.\n";
	glDepthMask(GL_TRUE);
}
