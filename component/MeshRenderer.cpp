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
#include "KeyboardInput.h"

float points[] = {
	0.0f,  0.5f,  -1.0f,
	-0.5f, -0.5f,  -1.0f,
	0.5f, -0.5f,  -1.0f
};

using namespace omen;
using namespace ecs;

MeshRenderer::MeshRenderer(MeshController* meshController) : 
	Renderer(), 
	m_specularCoeff(512), 
	m_shininess(5), 
	m_lightDir({ 0,1,0 }), 
	m_texture(nullptr), 
	m_renderNormals(false),
	m_meshController(meshController)
{
	m_shader = std::make_unique<omen::Shader>("shaders/pass_through_with_shadow.glsl");
}

void MeshRenderer::connectSlider(ui::Slider* slider) {
	slider->signal_slider_dragged.connect([this](ui::Slider* slider, float value)->void {
		std::cout << "Dragged: " << value << std::endl;
		setShininess(value);
	});
}

void MeshRenderer::onAttach(Entity* e) {
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
	/*Engine::instance()->window()->signal_file_dropped.connect([this](const std::vector<std::string>& files)
	{
		if( !files.empty() && files.begin()->find(".md3") == std::string::npos )
			m_texture = new Texture(*files.begin());
	});*/
	m_texture = new Texture("textures/checker.jpg");
	m_textureNormal = new Texture("textures/brick_normal.png");
	if (m_meshController) {
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
		std::vector<omen::Mesh::Vertex> &verts = m_meshController->mesh()->vertices();
		std::vector<glm::vec3> vertices;
		for (auto& v : verts) vertices.push_back(v.pos);
		check_gl_error();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
		check_gl_error();
		glVertexAttribPointer(VERTEX_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, 0);
		check_gl_error();
		glDisableVertexAttribArray(VERTEX_ATTRIB_POS);

		// Check if we have index buffer
		m_indexBufferSize = m_meshController->mesh()->vertexIndices().size();
		if (m_indexBufferSize>0) {
			glGenBuffers(1, &m_indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferSize * sizeof(unsigned int), m_meshController->mesh()->vertexIndices().data(), GL_STATIC_DRAW);
		}
		// Create VBO for texture
		std::vector<glm::vec2> uvs = m_meshController->mesh()->uv();
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
		std::vector<glm::vec3> normals = m_meshController->mesh()->normals();
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
		std::vector<glm::vec3> tangents = m_meshController->mesh()->tangents();
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

		// Create VBO for tangents
		std::vector<glm::vec3> bitangents = m_meshController->mesh()->bitangents();
		if (!bitangents.empty()) {
			glGenBuffers(1, &m_vbo_bitangents);
			check_gl_error();
			glEnableVertexAttribArray(VERTEX_ATTRIB_BITANGENT);
			check_gl_error();
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bitangents);
			check_gl_error();

			check_gl_error();
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*bitangents.size(), bitangents.data(), GL_STATIC_DRAW);
			check_gl_error();
			glVertexAttribPointer(VERTEX_ATTRIB_BITANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);
			check_gl_error();
			glDisableVertexAttribArray(VERTEX_ATTRIB_BITANGENT);
		}

		m_texture = m_meshController->mesh()->material() != nullptr ? m_meshController->mesh()->material()->texture() : nullptr;
	}

	Engine::instance()->findComponent<KeyboardInput>()->signal_key_press.connect([this](int key, int scan, int action, int mods)
	{
		int isN = key == GLFW_KEY_N;
		int isShift = mods & GLFW_MOD_SHIFT;

		if (isN && isShift)
		{
			m_renderNormals = !m_renderNormals;
		}
	});
}

void MeshRenderer::onDetach(Entity* e) {

}

void MeshRenderer::render(Shader* shader)
{	
	// Initialize shader with global parameters
	Shader* pShader = shader;
	if (pShader == nullptr) {
		pShader = m_shader.get();
		pShader->use();
	}

	// Get matrices
	glm::mat4 viewMatrix		= Engine::instance()->camera()->view();
	glm::mat4 viewprojMatrix	= Engine::instance()->camera()->viewProjection();
	glm::mat4 modelMatrix		= entity()->getComponent<Transform>()->tr();
	glm::mat4 modelViewMatrix	= viewMatrix * modelMatrix;
	glm::mat4 MVP = viewprojMatrix * modelMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(modelViewMatrix));
	glm::vec3 viewPos = Engine::instance()->camera()->position();
		
	glm::vec4 diffuseColor = m_meshController->mesh()->material()->const_diffuseColor();
	pShader->setUniform4fv("MaterialDiffuse", 1, glm::value_ptr(diffuseColor));
	
	// Set matrix uniforms
	pShader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(MVP), false);

	// Set other uniforms

	/*Setup the DepthMVP*/
	// Compute the MVP matrix from the light's point of view
	glm::vec3 lightInvDir = glm::vec3(-20, 20, -20);
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, 0, 100);
	glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// or, for spot light :
	//glm::vec3 lightPos(5, 20, 20);
	//glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
	//glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));

	glm::mat4 depthModelMatrix = glm::mat4(1.0);
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);

	glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	pShader->setUniformMatrix4fv("DepthBiasMVP", 1, glm::value_ptr(depthBiasMVP), false);
	/********************/

	pShader->setUniform1i("RenderNormals", m_renderNormals);

	if (m_texture != nullptr)
	{
		pShader->setUniform1i("HasTexture", 1);
		m_texture->bind();
	}
	else
	{
		pShader->setUniform1i("HasTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		omen::Engine* engine = omen::Engine::instance();
		GLint depthMap = engine->findSystem<omen::ecs::GraphicsSystem>()->depthMap;
		glBindTexture(GL_TEXTURE_2D, depthMap);
		//shader->setUniform1i("shadowMap", 1);
		/*GLuint ShadowMapID = glGetUniformLocation(m_shader->m_shader_program, "shadowMap");
		if (ShadowMapID > 0) {
			glUniform1i(ShadowMapID, 1);
		}*/

	}

	// Setup buffers for rendering
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

	if (m_vbo_bitangents != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bitangents);
		glEnableVertexAttribArray(VERTEX_ATTRIB_BITANGENT);
	}
	// draw points 0-3 from the currently bound VAO with current in-use shader
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	//glPolygonMode(GL_BACK, GL_LINE);
	if(m_indexBuffer!=0)
		glDrawElements(GL_TRIANGLES, m_indexBufferSize, GL_UNSIGNED_INT, (void*)0);
	else
		glDrawArrays(GL_TRIANGLES, 0, m_meshController->mesh()->vertices().size());
}
