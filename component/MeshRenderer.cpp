#include "Sprite.h"
#include "MeshRenderer.h"
#include "MeshController.h"
#include "../Entity.h"
#include "../Engine.h"
#include "../GL_error.h"
#include <glm/gtc/type_ptr.hpp>
#include "../ui/Slider.h"
#include "../ui/Slider.h"

float points[] = {
	0.0f,  0.5f,  -1.0f,
	-0.5f, -0.5f,  -1.0f,
	0.5f, -0.5f,  -1.0f
};

std::unique_ptr<omen::Shader> pShader = nullptr;

using namespace omen;
using namespace ecs;

MeshRenderer::MeshRenderer() : Renderer(), m_specularCoeff(512), m_shininess(5), m_lightDir({ 0,1,0 }), m_texture(nullptr)
{
	pShader = std::make_unique<omen::Shader>("shaders/pass_through.glsl");
}

void MeshRenderer::connectSlider(ui::Slider* slider) {
	slider->signal_slider_dragged.connect([this](ui::Slider* slider, float value)->void {
		std::cout << "Dragged: " << value << std::endl;
		setShininess(value);
	});
}

void MeshRenderer::onAttach(Entity* e) {
	const ecs::MeshController* meshController = e->getComponent<ecs::MeshController>();

	ui::Slider* slider = dynamic_cast<ui::Slider*>(Engine::instance()->scene()->findEntity("Slider0"));
	if (slider != nullptr) {
		connectSlider(slider);
	}
	else {
		Engine::instance()->scene()->signal_entity_added.connect([this](Entity* e) -> void {
			ui::Slider* slider = dynamic_cast<ui::Slider*>(e);
			if (slider != nullptr) {
				std::function<void(float value)> lambda;
				if (e->name() == "Slider0")
					lambda = [this](float value) -> void {setShininess(value); };
				if (e->name() == "Slider1")
					lambda = [this](float value) -> void {m_lightDir.x = -1.0f + 2.0f*value;};
				if (e->name() == "Slider2")
					lambda = [this](float value) -> void {m_lightDir.y = value; };
				if (e->name() == "Slider3")
					lambda = [this](float value) -> void {m_lightDir.z = -1.0f + 2.0f*value; };
				if (e->name() == "Slider4")
					lambda = [this](float value) -> void {m_specularCoeff = 1024.0f*value; };

				slider->signal_slider_dragged.connect([lambda](ui::Slider* slider, float value)->void {
					if(lambda!=nullptr)
						lambda(value);
				});
			}
		});
	}
	

	// Create Texture
	/*Engine::instance()->window()->signal_file_dropped.connect([this](std::vector<std::string>& files)
	{
		if( !files.empty() && files.begin()->find(".md3") == std::string::npos )
			m_texture = new Texture(*files.begin());
	})*/;
	m_texture = new Texture("textures/checker.jpg");
	m_textureNormal = new Texture("textures/brick_normal.png");
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
		std::vector<omen::Mesh::Vertex> &verts = meshController->mesh()->vertices();
		std::vector<glm::vec3> vertices;
		for (auto& v : verts) vertices.push_back(v.pos);
		check_gl_error();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
		check_gl_error();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		check_gl_error();
		glDisableVertexAttribArray(0);

		// Create VBO for texture
		std::vector<glm::vec2> uvs = meshController->mesh()->uv();

		if(!uvs.empty()){
			glGenBuffers(1, &m_vbo_texture);
			check_gl_error();
			glEnableVertexAttribArray(1);
			check_gl_error();
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texture);
			check_gl_error();
		
			check_gl_error();
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*uvs.size(), uvs.data(), GL_STATIC_DRAW);
			check_gl_error();
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			check_gl_error();
			glDisableVertexAttribArray(1);
		}
		
		// Create VBO for normals
		std::vector<glm::vec3> normals = meshController->mesh()->normals();
		if (!normals.empty()) {
			glGenBuffers(1, &m_vbo_normals);
			check_gl_error();
			glEnableVertexAttribArray(2);
			check_gl_error();
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
			check_gl_error();
		
			check_gl_error();
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), normals.data(), GL_STATIC_DRAW);
			check_gl_error();
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			check_gl_error();
			glDisableVertexAttribArray(2);
		}

		// Create VBO for tangents
		std::vector<glm::vec3> tangents = meshController->mesh()->tangents();
		if (!tangents.empty()) {
			glGenBuffers(1, &m_vbo_tangents);
			check_gl_error();
			glEnableVertexAttribArray(3);
			check_gl_error();
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_tangents);
			check_gl_error();

			check_gl_error();
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tangents.size(), tangents.data(), GL_STATIC_DRAW);
			check_gl_error();
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			check_gl_error();
			glDisableVertexAttribArray(3);
		}
	}
}

void MeshRenderer::onDetach(Entity* e) {

}

void MeshRenderer::render()
{		
	pShader->use();
	glm::vec3 viewPos = Engine::instance()->camera()->position();
	pShader->setUniform3fv("ViewPos", 1, glm::value_ptr(viewPos) );
	glm::mat4 model = entity()->getComponent<Transform>()->tr();
	pShader->setUniformMatrix4fv("Model", 1, glm::value_ptr(model), false);

	pShader->setUniform1f("Shininess", m_shininess);
	pShader->setUniform1f("SpecularCoeff", m_specularCoeff);
		
	if(m_texture != nullptr)
		m_texture->bind();

	glm::mat4 modelviewproj = Engine::instance()->camera()->viewProjection();
	glm::mat3 normalMatrix = glm::mat3(transpose(inverse(modelviewproj)));
	pShader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(modelviewproj), false);
	pShader->setUniformMatrix4fv("NormalMatrix", 1, glm::value_ptr(normalMatrix), false);
	pShader->setUniform3fv("LightDir", 1, glm::value_ptr(m_lightDir));
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(0);

	if (m_vbo_texture != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texture);
		glEnableVertexAttribArray(1);
	}

	if (m_vbo_normals != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
		glEnableVertexAttribArray(2);
	}

	/*if (m_vbo_tangents != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_tangents);
		glEnableVertexAttribArray(3);
	}*/
	// draw points 0-3 from the currently bound VAO with current in-use shader
	const ecs::MeshController* meshController = entity()->getComponent<ecs::MeshController>();
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 0, meshController->mesh()->vertices().size());
}
