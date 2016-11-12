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

void omen::ecs::MeshRenderer::onAttach(std::unique_ptr<Entity> e) {
	const ecs::MeshController* meshController = nullptr; // e->getComponent<ecs::MeshController>();
	if (meshController) {
		// Create VAO
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Create VBO
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, meshController->mesh()->vertices().size(), meshController->mesh()->vertices().data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}
}

void omen::ecs::MeshRenderer::onDetach(std::unique_ptr<Entity> e) {

}

void omen::ecs::MeshRenderer::render()
{
	check_gl_error();
	glClear(GL_DEPTH_BUFFER_BIT);
		
	pShader->use();
	glBindVertexArray(m_vao);

	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
