#include "Sprite.h"
#include "MeshRenderer.h"
#include "MeshController.h"
#include "../Entity.h"
#include "../Engine.h"
#include "../GL_error.h"
#include <glm/gtc/type_ptr.hpp>

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
	// Create Texture
	m_texture = new Texture("textures/sirius.jpg");
	m_sprite = new Sprite("textures/skull.png", glm::vec2(0,0), 100, 100);
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
		glEnableVertexAttribArray(0);
		check_gl_error();
		std::vector<glm::vec3> verts = meshController->mesh()->vertices();
		check_gl_error();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*verts.size(), verts.data(), GL_STATIC_DRAW);
		check_gl_error();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		check_gl_error();
		glDisableVertexAttribArray(0);

		// Create VBO for texture
		glGenBuffers(1, &m_vbo_texture);
		check_gl_error();
		glEnableVertexAttribArray(1);
		check_gl_error();
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texture);
		check_gl_error();
		std::vector<glm::vec2> uvs = meshController->mesh()->uv();
		check_gl_error();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*uvs.size(), uvs.data(), GL_STATIC_DRAW);
		check_gl_error();
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		check_gl_error();
		glDisableVertexAttribArray(1);

		// Create VBO for texture
		glGenBuffers(1, &m_vbo_normals);
		check_gl_error();
		glEnableVertexAttribArray(2);
		check_gl_error();
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
		check_gl_error();
		std::vector<glm::vec3> normals = meshController->mesh()->normals();
		check_gl_error();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), normals.data(), GL_STATIC_DRAW);
		check_gl_error();
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		check_gl_error();
		glDisableVertexAttribArray(2);

	}
}

void omen::ecs::MeshRenderer::onDetach(Entity* e) {

}

void omen::ecs::MeshRenderer::render()
{
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	glViewport(0, 0, w, h);

	// TODO
	/*for (int i = 0; i < 4; ++i) {
		if (i == 0) {
			glViewport(0, 0, w / 2, h /2);
		}
		if (i == 1) {
			glViewport(w / 2, 0, w / 2, h /2);
		}
		if (i == 2) {
			glViewport(0, h/2, w / 2, h / 2);
		}
		if (i == 3) {
			glViewport(w / 2, h/2, w / 2, h / 2);
		}*/
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glDisable(GL_CULL_FACE);
		//glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
		check_gl_error();
		glClear(GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		
		pShader->use();
		glm::vec3 viewPos = Engine::instance()->camera()->position();
		pShader->setUniform3fv("ViewPos", 1, glm::value_ptr(viewPos) );
		glm::mat4 model;
		GLfloat angle = 20.0f * Engine::instance()->time()*0.1f;
		model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
		pShader->setUniformMatrix4fv("Model", 1, glm::value_ptr(model), false);

		m_texture->bind();
		//m_texture->bindSampler();

		glm::mat4 modelviewproj = Engine::instance()->camera()->viewProjection();
		pShader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(modelviewproj), false);
		check_gl_error();
		glBindVertexArray(m_vao);
		check_gl_error();

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texture);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
		glEnableVertexAttribArray(2);
		// draw points 0-3 from the currently bound VAO with current in-use shader
		const ecs::MeshController* meshController = entity()->getComponent<ecs::MeshController>();
		glDrawArrays(GL_TRIANGLES, 0, meshController->mesh()->vertices().size());
		check_gl_error();	
	//}
		m_sprite->render();
}
