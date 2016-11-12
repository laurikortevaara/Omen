#include "MeshRenderer.h"
#include "MeshController.h"
#include "../GL_error.h"

float points[] = {
	0.0f,  0.5f,  -1.0f,
	-0.5f, -0.5f,  -1.0f,
	0.5f, -0.5f,  -1.0f
};

std::unique_ptr<omen::Shader> pShader = nullptr;

omen::ecs::MeshRenderer::MeshRenderer() : Renderer()
{
	pShader = std::make_unique<omen::Shader>("shaders/pass_through.glsl");
}

void omen::ecs::MeshRenderer::onAttach(Entity* e) {
	const ecs::MeshController* meshController = e->getComponent<ecs::MeshController>();
	if (meshController) {
		// Create VAO
		check_gl_error();
		glGenVertexArrays(1, &m_vao);
		check_gl_error();
		glBindVertexArray(m_vao);
		check_gl_error();

		// Create VBO
		glGenBuffers(1, &m_vbo);
		check_gl_error();
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		check_gl_error();
		std::vector<glm::vec3> verts = meshController->mesh()->vertices();
		check_gl_error();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*verts.size(), verts.data(), GL_STATIC_DRAW);
		check_gl_error();

		glEnableVertexAttribArray(0);
		check_gl_error();
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		check_gl_error();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		check_gl_error();
	}
}

void omen::ecs::MeshRenderer::onDetach(Entity* e) {

}

void omen::ecs::MeshRenderer::render()
{
	glDisable(GL_CULL_FACE);
	check_gl_error();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		
	pShader->use();
	glm::mat4x4 modelviewproj = Engine::instance()->camera()->viewProjection();
	pShader->setUniformMatrix4fv("ModelViewProjection", 1, (float*)&modelviewproj, false);
	check_gl_error();
	glBindVertexArray(m_vao);
	check_gl_error();

	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawArrays(GL_TRIANGLES, 0, 3);
	check_gl_error();
}
