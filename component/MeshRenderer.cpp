#include "Sprite.h"
#include "MeshRenderer.h"
#include "MeshController.h"
#include "../Entity.h"
#include "../Engine.h"
#include "../GL_error.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "../system/OpenVRSystem.h"
#include "../ui/Slider.h"
#include "../ui/Slider.h"
#include "KeyboardInput.h"
#include "Picker.h"

float points[] = {
	0.0f,  0.5f,  -1.0f,
	-0.5f, -0.5f,  -1.0f,
	0.5f, -0.5f,  -1.0f
};


static GLuint blend_modes[] = {
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_CONSTANT_ALPHA,
	GL_ONE_MINUS_CONSTANT_ALPHA
};

float spotlight_distance = 0.1f;
float spotlight_angle = 45.0f;
float spotlight_near = 0.55f;
float spotlight_far = 100.0f;
float depth_bias = 0.01f;

int selector = 1;


using namespace omen;
using namespace ecs;

MeshRenderer::MeshRenderer(MeshController* meshController) :
	Renderer(),
	m_specularCoeff(512),
	m_shininess(5),
	m_lightDir({ 0,1,0 }),
	m_texture(nullptr),
	m_renderNormals(false),
	m_meshController(meshController),
	m_shaderBlur(1)
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
					lambda = [this](float value) -> void {m_lightDir.x = -1.0f + 2.0f*value; };
				if (e->name() == "Slider2")
					lambda = [this](float value) -> void {m_lightDir.y = value; };
				if (e->name() == "Slider3")
					lambda = [this](float value) -> void {m_lightDir.z = -1.0f + 2.0f*value; };
				if (e->name() == "Slider4")
					lambda = [this](float value) -> void {m_specularCoeff = 1024.0f*value; };

				slider->signal_slider_dragged.connect([lambda](ui::Slider* slider, float value)->void {
					if (lambda != nullptr)
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
	/*m_texture = new Texture("textures/checker.jpg");
	m_textureNormal = new Texture("textures/brick_normal.png");*/
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
		if (!uvs.empty()) {
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
			m_texture = m_meshController->mesh()->material()->texture();
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
		std::cout << "MeshRenderer:signa_key_press" << std::endl;
		if (action == GLFW_KEY_DOWN)
			return;
		int isN = key == GLFW_KEY_N;
		int isShift = mods & GLFW_MOD_SHIFT;

		if (isN && isShift)
		{
			m_renderNormals = !m_renderNormals;
		}

		if (key == GLFW_KEY_1)
			selector = 1;

		if (key == GLFW_KEY_2)
			selector = 2;

		if (key == GLFW_KEY_3)
			selector = 3;

		if (key == GLFW_KEY_4)
			selector = 4;

		if (key == GLFW_KEY_5)
			selector = 5;

		switch (selector)
		{
		case 1:
			if (key == GLFW_KEY_PAGE_UP)
				spotlight_distance += 0.001f;
			if (key == GLFW_KEY_PAGE_DOWN)
				spotlight_distance -= 0.001f;
			break;
		case 2:
			if (key == GLFW_KEY_PAGE_UP)
				spotlight_angle += 5.0f;
			if (key == GLFW_KEY_PAGE_DOWN)
				spotlight_angle -= 5.0f;
			break;
		case 3:
			if (key == GLFW_KEY_PAGE_UP)
				spotlight_near += 0.01f;
			if (key == GLFW_KEY_PAGE_DOWN)
				spotlight_near -= 0.01f;
			break;
		case 4:
			if (key == GLFW_KEY_PAGE_UP)
				spotlight_far += 0.1f;
			if (key == GLFW_KEY_PAGE_DOWN)
				spotlight_far -= 0.1f;
			break;
		case 5:
			if (key == GLFW_KEY_PAGE_UP)
				depth_bias += 0.01f;
			if (key == GLFW_KEY_PAGE_DOWN)
				depth_bias -= 0.01f;
			break;
		}

		spotlight_angle = glm::min(180.0f, spotlight_angle);
		spotlight_angle = glm::max(0.0f, spotlight_angle);
		spotlight_near = glm::max(0.0f, spotlight_near);
		spotlight_far = glm::max(spotlight_near, spotlight_far);
		depth_bias = glm::max(0.0f, depth_bias);

		m_shaderBlur = glm::clamp(m_shaderBlur, 1, 20);
	});
}

void MeshRenderer::onDetach(Entity* e) {

}

void MeshRenderer::render(Shader* shader)
{
	if (!Engine::instance()->meshRendererEnabled())
		return;
	Shader* pShader = shader == nullptr ? m_shader.get() : shader;

	pShader->use();

	
	// Get matrices
	ecs::OpenVRSystem* vrsys = Engine::instance()->findSystem<ecs::OpenVRSystem>();
	glm::mat4 MVP = glm::mat4(1);
	if (vrsys != nullptr && vrsys->renderVR())
	{
		MVP = vrsys->getCurrentViewProjectionMatrix(vrsys->currentEye()) * entity()->tr()->tr();
		pShader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(MVP), false);
	}
	else
	{
		glm::mat4 viewMatrix = Engine::instance()->camera()->view();
		glm::mat4 viewprojMatrix = Engine::instance()->camera()->viewProjection();
		MVP = viewprojMatrix * entity()->tr()->tr();

		// Set matrix uniforms
		pShader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(MVP), false);
		glm::mat4 inverseMVP = glm::inverse(viewMatrix*entity()->tr()->tr());
		pShader->setUniformMatrix4fv("InverseMVP", 1, glm::value_ptr(inverseMVP), false);

		glm::mat4 inverseViewProjMatrix = glm::inverse(viewprojMatrix);
		glm::mat4 modelMatrix = entity()->getComponent<Transform>()->tr();
		glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
		glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(modelViewMatrix));
		glm::vec3 viewPos = Engine::instance()->camera()->position();

		glm::vec4 diffuseColor = m_meshController->mesh()->material()->const_diffuseColor();
		pShader->setUniform4fv("MaterialDiffuse", 1, glm::value_ptr(diffuseColor));

		// Set matrix uniforms
		pShader->setUniformMatrix4fv("InverseViewProjection", 1, glm::value_ptr(inverseViewProjMatrix), false);
		pShader->setUniform1f("DepthBias", depth_bias);
		// Set other uniforms

		/*Setup the DepthMVP*/
		// Compute the MVP matrix from the light's point of view
		glm::vec3 lightPos = Engine::LightDistance*Engine::LightPos;
		glm::vec3 lightInvDir = normalize(-lightPos);
		pShader->setUniform3fv("LightPos", 1, glm::value_ptr(lightPos));
		float size = Engine::ShadowFrustumSize;
		glm::mat4 depthProjectionMatrix = glm::ortho<float>(-size, size, -size, size, Engine::ShadowFrustumNear, Engine::ShadowFrustumFar);
		glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));

		glm::mat4 depthModelMatrix = entity()->getComponent<Transform>()->tr();
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
		pShader->setUniformMatrix4fv("depthMVP", 1, glm::value_ptr(depthMVP), false);
		pShader->setUniformMatrix4fv("DepthBiasMVP", 1, glm::value_ptr(depthBiasMVP), false);
		pShader->setUniformMatrix4fv("ModelView", 1, glm::value_ptr(modelViewMatrix), false);
		pShader->setUniform3fv("CameraPosition", 1, glm::value_ptr(viewPos));
		pShader->setUniform1i("IsSelected", entity()->selected());
		/********************/
	}

	pShader->setUniform1i("RenderNormals", m_renderNormals); 
	pShader->setUniform1i("ShadowBlur", Engine::ShadowBlur);
	pShader->setUniform1f("MaterialShininess", Engine::MaterialShininess);
	pShader->setUniform1f("LightIntensity", Engine::LightIntensity);
	pShader->setUniform1f("AmbientFactor", Engine::AmbientFactor);
	std::cout << "Ipos: " << Picker::IntersectPos.x << ", " << Picker::IntersectPos.y << ", " << Picker::IntersectPos.z << "\n";
	pShader->setUniform3fv("IntersectPos", 1, glm::value_ptr(Picker::IntersectPos));

	if (m_texture != nullptr && pShader == m_shader.get())
	{
		pShader->setUniform1i("HasTexture", true);

		int textureLoc = pShader->getUniformLocation("TextureMap");
		glUniform1i(textureLoc, 0);

		glActiveTexture(GL_TEXTURE0);
		m_texture->bind();
	}
	{
		//pShader->setUniform1i("HasTexture", false);
		omen::Engine* engine = omen::Engine::instance();
		GLint depthMap = engine->findSystem<omen::ecs::GraphicsSystem>()->depthMap;

		int shadowLoc = pShader->getUniformLocation("shadowMap");
		glUniform1i(shadowLoc, 1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
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

	//glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	if (m_indexBuffer != 0)
		drawElementsInstanced(GL_TRIANGLES, m_indexBufferSize, GL_UNSIGNED_INT, (void*)0, 1);
	else
		drawArrays(GL_TRIANGLES, 0, m_meshController->mesh()->vertices().size());

}
