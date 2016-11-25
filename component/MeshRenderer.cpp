#include "Sprite.h"
#include "MeshRenderer.h"
#include "MeshController.h"
#include "../Entity.h"
#include "../Engine.h"
#include "../GL_error.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "../ui/Slider.h"
#include "../ui/Slider.h"

#define VERTEX_ATTRIB_POS 0
#define VERTEX_ATTRIB_TCOORD 1
#define VERTEX_ATTRIB_NORMAL 2
#define VERTEX_ATTRIB_TANGENT 3

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
		glEnableVertexAttribArray(VERTEX_ATTRIB_POS);
		check_gl_error();
		std::vector<omen::Mesh::Vertex> &verts = meshController->mesh()->vertices();
		std::vector<glm::vec3> vertices;
		for (auto& v : verts) vertices.push_back(v.pos);
		check_gl_error();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
		check_gl_error();
		glVertexAttribPointer(VERTEX_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, 0);
		check_gl_error();
		glDisableVertexAttribArray(VERTEX_ATTRIB_POS);

		// Create VBO for texture
		std::vector<glm::vec2> uvs = meshController->mesh()->uv();

		if(!uvs.empty()){
			glGenBuffers(1, &m_vbo_texture);
			check_gl_error();
			glEnableVertexAttribArray(VERTEX_ATTRIB_TCOORD);
			check_gl_error();
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texture);
			check_gl_error();
		
			check_gl_error();
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*uvs.size(), uvs.data(), GL_STATIC_DRAW);
			check_gl_error();
			glVertexAttribPointer(VERTEX_ATTRIB_TCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
			check_gl_error();
			glDisableVertexAttribArray(VERTEX_ATTRIB_TCOORD);
		}
		
		// Create VBO for normals
		std::vector<glm::vec3> normals = meshController->mesh()->normals();
		if (!normals.empty()) {
			glGenBuffers(1, &m_vbo_normals);
			check_gl_error();
			glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL);
			check_gl_error();
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
			check_gl_error();
		
			check_gl_error();
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), normals.data(), GL_STATIC_DRAW);
			check_gl_error();
			glVertexAttribPointer(VERTEX_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
			check_gl_error();
			glDisableVertexAttribArray(VERTEX_ATTRIB_NORMAL);
		}

		// Create VBO for tangents
		std::vector<glm::vec3> tangents = meshController->mesh()->tangents();
		if (!tangents.empty()) {
			glGenBuffers(1, &m_vbo_tangents);
			check_gl_error();
			glEnableVertexAttribArray(VERTEX_ATTRIB_TANGENT);
			check_gl_error();
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_tangents);
			check_gl_error();

			check_gl_error();
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tangents.size(), tangents.data(), GL_STATIC_DRAW);
			check_gl_error();
			glVertexAttribPointer(VERTEX_ATTRIB_TANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);
			check_gl_error();
			glDisableVertexAttribArray(VERTEX_ATTRIB_TANGENT);
		}
	}
}

void MeshRenderer::onDetach(Entity* e) {

}

void MeshRenderer::render()
{		
	// Use shader
	pShader->use();

	// Get matrices
	glm::mat4 viewMatrix		= Engine::instance()->camera()->view();
	glm::mat4 viewprojMatrix	= Engine::instance()->camera()->viewProjection();
	glm::mat4 modelMatrix		= entity()->getComponent<Transform>()->tr();
	glm::mat4 modelViewMatrix	= viewMatrix * modelMatrix;
	glm::mat4 MVP = viewprojMatrix * modelMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(modelViewMatrix));
	glm::vec3 viewPos = Engine::instance()->camera()->position();

	// Set matrix uniforms
	pShader->setUniformMatrix4fv("ViewMatrix", 1, glm::value_ptr(viewMatrix), false);
	pShader->setUniformMatrix4fv("ViewProjMatrix", 1, glm::value_ptr(modelMatrix), false);
	pShader->setUniformMatrix4fv("ModelMatrix", 1, glm::value_ptr(normalMatrix), false);
	pShader->setUniformMatrix4fv("ModelViewMatrix", 1, glm::value_ptr(modelViewMatrix), false);
	pShader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(MVP), false);
	pShader->setUniformMatrix3fv("NormalMatrix", 1, glm::value_ptr(normalMatrix), false);

	// Set other uniforms
	pShader->setUniform3fv("ViewPos", 1, glm::value_ptr(viewPos));
	pShader->setUniform1f("Shininess", m_shininess);
	pShader->setUniform1f("SpecularCoeff", m_specularCoeff);
	pShader->setUniform3fv("LightDir", 1, glm::value_ptr(m_lightDir));

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(VERTEX_ATTRIB_POS);

	if (m_vbo_texture != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texture);
		glEnableVertexAttribArray(VERTEX_ATTRIB_TCOORD);
	}

	if (m_vbo_normals != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
		glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL);
	}

	if (m_vbo_tangents != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_tangents);
		glEnableVertexAttribArray(VERTEX_ATTRIB_TANGENT);
	}
	// draw points 0-3 from the currently bound VAO with current in-use shader

	if (m_texture != nullptr)
		m_texture->bind();
	
	const ecs::MeshController* meshController = entity()->getComponent<ecs::MeshController>();

	glDrawArrays(GL_TRIANGLES, 0, meshController->mesh()->vertices().size());
}
