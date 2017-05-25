#include <limits>

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
#include "../ui/TextView.h"
#include "../SkyBoxRenderer.h"

float points[] = {
	0.0f,  0.5f,  -1.0f,
	-0.5f, -0.5f,  -1.0f,
	0.5f, -0.5f,  -1.0f
};

glm::vec3 lightInvDir = glm::normalize( glm::vec3(100, -100.0, 50.0) );

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
	m_textureNormal(nullptr),
	m_textureSpecularColor(nullptr),
	m_renderNormals(false),
	m_meshController(meshController),
	m_shaderBlur(1)
{
	m_shader = std::make_unique<omen::Shader>("shaders/pass_through_with_shadow.glsl");

	Engine::instance()->signal_engine_update.connect(this,[this](float t, float dt) {
		lightInvDir = glm::normalize(glm::vec3(100, -100.0, 50.0));
		lightInvDir.x *= cos(t);
		lightInvDir.z *= sin(t);
		//lightInvDir = -Engine::instance()->scene()->lights()[0]->position();
	});
}

void MeshRenderer::connectSlider(ui::Slider* slider) {
	slider->signal_slider_dragged.connect(this,[this](ui::Slider* slider, float value)->void {
		std::cout << "Dragged: " << value << std::endl;
		setShininess(value);
	});
}

void MeshRenderer::onAttach(Entity* e) {
	m_entity = e;
	ui::Slider* slider = dynamic_cast<ui::Slider*>(Engine::instance()->scene()->findEntity("Slider0"));
	if (slider != nullptr) {
		connectSlider(slider);
	}
	else {
		Engine::instance()->scene()->signal_entity_added.connect(this,[this](Entity* e) -> void {
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

				slider->signal_slider_dragged.connect(this,[lambda](ui::Slider* slider, float value)->void {
					if (lambda != nullptr)
						lambda(value);
				});
			}
		});
	}

	// Create Texture
	/*Engine::instance()->window()->signal_file_dropped.connect(this,[this](const std::vector<std::string>& files)
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
			m_textureNormal = m_meshController->mesh()->material()->textureNormal();
			m_textureSpecularColor = m_meshController->mesh()->material()->textureSpecularColor();
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

		const std::unique_ptr<Mesh>& mesh = m_meshController->mesh();
		const std::unique_ptr<Material>& material = mesh->material();
		omen::Texture* texture = material->texture();
		m_texture = m_meshController->mesh()->material() != nullptr ? m_meshController->mesh()->material()->texture() : nullptr;
	}

	Engine::instance()->findComponent<KeyboardInput>()->signal_key_press.connect(this,[this](int key, int scan, int action, int mods)
	{
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

void CalcProj(glm::mat4& view, glm::vec3& lightDir, glm::mat4& proj)
{
	glm::mat4 lightView = glm::lookAt( glm::vec3(0), lightDir, glm::vec3(0, 1, 0));

	Window* win = Engine::instance()->window();
	Camera* cam = Engine::instance()->camera();
	float nearPlane = -cam->zNear();
	float farPlane = cam->zFar();
	float fov = glm::radians(cam->fov());
	int width = win->width();
	int height = win->height();

	glm::vec4 frustumMin(std::numeric_limits<float>::max());
	glm::vec4 frustumMax(std::numeric_limits<float>::lowest());

	const float nearHeight = 2.0f * tan(fov * 0.5f) * nearPlane;
	const float nearWidth = nearHeight * static_cast<float>(width) / height;
	const float farHeight = 2.0f * tan(fov * 0.5f) * farPlane;
	const float farWidth = farHeight * static_cast<float>(width) / height;
	const glm::vec4 cameraPos = glm::vec4(cam->position(),1); // nv::inverse(m_camera.getTranslationMat()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	const glm::mat4 invRot = cam->rotation();
	const glm::vec4 viewDir = invRot * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	const glm::vec4 upDir = invRot * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	const glm::vec4 rightDir = invRot * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	const glm::vec4 nc = cameraPos + viewDir * nearPlane; // near center
	const glm::vec4 fc = cameraPos + viewDir * farPlane; // far center

														 // Vertices in a world space.
	glm::vec4 vertices[8] = {
		nc - upDir * nearHeight * 0.5f - rightDir * nearWidth * 0.5f, // nbl (near, bottom, left)
		nc - upDir * nearHeight * 0.5f + rightDir * nearWidth * 0.5f, // nbr
		nc + upDir * nearHeight * 0.5f + rightDir * nearWidth * 0.5f, // ntr
		nc + upDir * nearHeight * 0.5f - rightDir * nearWidth * 0.5f, // ntl
		fc - upDir * farHeight  * 0.5f - rightDir * farWidth * 0.5f, // fbl (far, bottom, left)
		fc - upDir * farHeight  * 0.5f + rightDir * farWidth * 0.5f, // fbr
		fc + upDir * farHeight  * 0.5f + rightDir * farWidth * 0.5f, // ftr
		fc + upDir * farHeight  * 0.5f - rightDir * farWidth * 0.5f, // ftl
	};


	for (unsigned int vertId = 0; vertId < 8; ++vertId) {
		// Light view space.
		vertices[vertId] = lightView * vertices[vertId];
		// Update bounding box.
		frustumMin = glm::min(frustumMin, vertices[vertId]);
		frustumMax = glm::max(frustumMax, vertices[vertId]);
	}
/*	std::cout << "Camera Frustum:";
	for (int i = 0; i < 8; ++i)
		std::cout << "[" << vertices[i].x << "," << vertices[i].y << "," << vertices[i].z << "," << vertices[i].w << "]\n";
	std::cout << "\nLight Frustum:";
	std::cout << "[" << frustumMin.x << "," << frustumMin.y << "," << frustumMin.z << "," << frustumMin.w << "]\n";
	std::cout << "[" << frustumMax.x << "," << frustumMax.y << "," << frustumMax.z << "," << frustumMax.w << "]\n";*/
	proj = glm::ortho(-frustumMin.x, -frustumMax.x, frustumMin.y, frustumMax.y, frustumMin.z, frustumMax.z);

	ui::TextView* tv = (ui::TextView*)Engine::instance()->scene()->findEntity("textview");
	WCHAR wc[512] = { '\0' };
	wcscpy_s(wc, L"Frustum:\n");
	for (int i = 0; i < 8; ++i)
		swprintf(wc, L"%s\n% 8.3f,  % 8.3f,  % 8.3f", wc, vertices[i].x, vertices[i].y, vertices[i].z);
	swprintf(wc, L"%s\n\nLFRMin/Max\n% 8.3f,  % 8.3f,  % 8.3f\n% 8.3f,  % 8.3f,  % 8.3f", wc, frustumMin.x, frustumMin.y, frustumMin.z, frustumMax.x, frustumMax.y, frustumMax.z);
	tv->setText(wc);
}

void MeshRenderer::render(Shader* shader)
{
	Shader* pShader = shader == nullptr ? m_shader.get() : shader;

	pShader->use();
	
	// Get matrices
	glm::mat4 viewMatrix			= Engine::instance()->camera()->view();
	glm::mat4 inverseViewMatrix		= glm::inverse(viewMatrix);
	glm::mat4 modelMatrix = entity()->getComponent<Transform>()->world_tr();
	glm::mat4 modelViewMatrix		= viewMatrix * modelMatrix;
	glm::mat4 viewprojMatrix		= Engine::instance()->camera()->viewProjection();
	glm::mat4 MVP					= viewprojMatrix * entity()->tr()->world_tr();
	glm::mat3 mv3x3					= glm::mat3(modelViewMatrix);
	glm::mat4 inverseMVP			= glm::inverse(viewMatrix*entity()->tr()->world_tr());
	glm::mat4 inverseViewProjMatrix = glm::inverse(viewprojMatrix);
	glm::mat3 normalMatrix			= glm::mat3(glm::inverseTranspose(modelViewMatrix));
	glm::vec3 viewPos				= Engine::instance()->camera()->position();
	glm::vec3 lightPos = lightInvDir; // Engine::LightPos;
	float size = 10.0f;
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-size, size, -size, size, 0.01f, 20.0f);// Engine::ShadowFrustumNear, Engine::ShadowFrustumFar);
	glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(0.0f), lightInvDir, glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = entity()->getComponent<Transform>()->world_tr();
	CalcProj(viewMatrix, lightInvDir, depthProjectionMatrix);
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);
	glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

	// Material properties
	const std::unique_ptr<Material>& m = m_meshController->mesh()->material();
	glm::vec4 ambientColor = m->ambientColor();
	glm::vec4 diffuseColor = m->diffuseColor();
	glm::vec4 emissiveColor = m->emissiveColor();
	glm::vec4 specularColor = m->specularColor();
	float diffuseIntensity = m->diffuseIntensity();
	float specularIntensity = m->specularIntensity();
	float materialShininess = m->shininess();

	// Matrices to shader
	pShader->setUniformMatrix4fv("InverseView", 1, glm::value_ptr(inverseViewMatrix), false);
	pShader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(MVP), false);
	pShader->setUniformMatrix3fv("ModelView3x3", 1, glm::value_ptr(mv3x3), false);
	pShader->setUniformMatrix4fv("ModelView", 1, glm::value_ptr(modelViewMatrix), false);
	pShader->setUniformMatrix4fv("ViewMatrix", 1, glm::value_ptr(viewMatrix), false);
	pShader->setUniformMatrix4fv("ModelMatrix", 1, glm::value_ptr(modelMatrix), false);
	pShader->setUniformMatrix4fv("InverseViewProjection", 1, glm::value_ptr(inverseViewProjMatrix), false);
	pShader->setUniformMatrix4fv("DepthBiasMVP", 1, glm::value_ptr(depthBiasMVP), false);
	pShader->setUniformMatrix4fv("depthMVP", 1, glm::value_ptr(depthMVP), false);
	
	// Light, camera and depth bias for shadow rendering
	pShader->setUniform1f("DepthBias", depth_bias);
	glm::vec3 lpos = 10.0f*glm::vec3(cos(Engine::instance()->time()),1,sin(Engine::instance()->time())); // Engine::instance()->scene()->lights()[0]->position();
	pShader->setUniform3fv("LightPos", 1, glm::value_ptr(lpos));
	pShader->setUniform3fv("ViewPos", 1, glm::value_ptr(viewPos));
	// Material properties to shader
	pShader->setUniform4fv("MaterialAmbient", 1, glm::value_ptr(ambientColor));
	pShader->setUniform4fv("MaterialDiffuse", 1, glm::value_ptr(diffuseColor));
	pShader->setUniform4fv("MaterialEmissive", 1, glm::value_ptr(emissiveColor));
	pShader->setUniform4fv("MaterialSpecular", 1, glm::value_ptr(specularColor));
	pShader->setUniform1f("DiffuseIntensity", diffuseIntensity);
	pShader->setUniform1f("SpecularIntensity", specularIntensity);
	pShader->setUniform1f("MaterialShininess", materialShininess);

	pShader->setUniform1i("HasTexture", false);
	int textureLoc = pShader->getUniformLocation("DiffuseColorMap");
	if (textureLoc >= 0 && m_texture != nullptr && m_texture->id() >= 0) {
		pShader->setUniform1i("HasDiffuseTexture", true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(textureLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture->id());
	}
	else
		pShader->setUniform1i("HasDiffuseTexture", false);

	textureLoc = pShader->getUniformLocation("NormalMap");
	if (textureLoc >= 0 && m_textureNormal != nullptr && m_textureNormal->id() >= 0) {
		pShader->setUniform1i("HasNormalTexture", true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(textureLoc, 1);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, m_textureNormal->id());
	}
	else
		pShader->setUniform1i("HasNormalTexture", false);

	textureLoc = pShader->getUniformLocation("SpecularColorMap");
	if (textureLoc >= 0 && m_textureSpecularColor != nullptr && m_textureSpecularColor->id() >= 0) {
		pShader->setUniform1i("HasSpecularColorTexture", true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(textureLoc, 2);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, m_textureSpecularColor->id());
	}
	else
		pShader->setUniform1i("HasSpecularColorTexture", false);

	int shadowLoc = pShader->getUniformLocation("shadowMap");
	if (shadowLoc >= 0) {
		omen::Engine* engine = omen::Engine::instance();
		GLint depthMap = engine->findSystem<omen::ecs::GraphicsSystem>()->depthMap;
		glUniform1i(shadowLoc, 3);
		glActiveTexture(GL_TEXTURE0+3);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}


	int envmapLoc = pShader->getUniformLocation("envMap");
	if (envmapLoc >= 0) {
		glUniform1i(envmapLoc, 4);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBoxRenderer::uvTexMap->id());
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

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	drawElementsInstanced(GL_TRIANGLES, m_indexBufferSize, GL_UNSIGNED_INT, (void*)0, 1);
}

void MeshRenderer::render2(Shader* shader)
{
	if (!Engine::instance()->meshRendererEnabled())
		return;
	Shader* pShader = shader == nullptr ? m_shader.get() : shader;

	pShader->use();

	glEnable(GL_BLEND);
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
		glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
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

		glm::vec4 diffuseColor = m_meshController->mesh()->material()->diffuseColor();
		pShader->setUniform4fv("MaterialDiffuse", 1, glm::value_ptr(diffuseColor));

		glm::vec4 emissiveColor = m_meshController->mesh()->material()->const_emissiveColor();
		pShader->setUniform4fv("MaterialEmissive", 1, glm::value_ptr(emissiveColor));

		// Set matrix uniforms
		pShader->setUniformMatrix4fv("InverseViewProjection", 1, glm::value_ptr(inverseViewProjMatrix), false);
		pShader->setUniform1f("DepthBias", depth_bias);
		// Set other uniforms

		/*Setup the DepthMVP*/
		// Compute the MVP matrix from the light's point of view
		glm::vec3 lightPos = Engine::instance()->scene()->lights()[0]->position();
		//glm::vec3 lightInvDir = normalize(-lightPos);
		pShader->setUniform3fv("LightPos", 1, glm::value_ptr(lightPos));
		float size = 10.0f; // Engine::ShadowFrustumSize;
		glm::mat4 depthProjectionMatrix = glm::ortho<float>(-size, size, -size, size, 0.01f, 20.0f);// Engine::ShadowFrustumNear, Engine::ShadowFrustumFar);
		glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(0.0f), lightInvDir, glm::vec3(0, 1, 0));

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

	pShader->setUniform1i("HasTexture", false);
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
		if (shadowLoc >= 0) {
			glUniform1i(shadowLoc, 1);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap);
		}
	}

	// Env map
	
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

void MeshRenderer::renderShadows(Shader* shader)
{
	shader->use();

	/*Setup the DepthMVP*/
	// Compute the MVP matrix from the light's point of view
	glm::vec3 lightPos = lightInvDir;
	float size = 10.0f;
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-size, size, -size, size, 0.01f, 20.0f);// Engine::ShadowFrustumNear, Engine::ShadowFrustumFar);
	glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(0.0f), lightInvDir, glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = entity()->getComponent<Transform>()->tr();
	glm::mat4 viewMatrix = Engine::instance()->camera()->view();
	glm::mat4 modelMatrix = entity()->getComponent<Transform>()->world_tr();
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	CalcProj(viewMatrix, lightInvDir, depthProjectionMatrix);
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
	shader->setUniformMatrix4fv("depthMVP", 1, glm::value_ptr(depthMVP), false);

	// Setup buffers for rendering
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(VERTEX_ATTRIB_POS);

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	if (m_indexBuffer != 0)
		drawElementsInstanced(GL_TRIANGLES, m_indexBufferSize, GL_UNSIGNED_INT, (void*)0, 1);
	else
		drawArrays(GL_TRIANGLES, 0, m_meshController->mesh()->vertices().size());
}
