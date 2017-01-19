#include "TerrainRenderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "component/MeshController.h"
#include "GL_error.h"

#include <sstream>
#include <iomanip>

using namespace omen;
using namespace ecs;

static glm::vec2 translations[100];
static Texture* heightmap[100];
static Texture* map_texture[100];

TerrainRenderer::TerrainRenderer(MeshController* mc) :
	MeshRenderer(mc)
{
	/*for (int i = 1; i < 100; ++i)
	{
		std::ostringstream ostr;
		ostr << "textures/map/heightmap_" << std::setw(2) << std::setfill('0') << i << ".png";
		std::string fname = ostr.str();
		heightmap[i - 1] = new Texture(fname);
	}
	heightmap[99] = new Texture("textures/map/heightmap_100.png");
	for (int i = 1; i < 100; ++i) 
	{
		std::ostringstream ostr;
		ostr << "textures/map/terrain_" << std::setw(2) << std::setfill('0') << i << ".png";
		std::string fname = ostr.str();
		map_texture[i-1] = new Texture(fname);
	}
	map_texture[99] = new Texture("textures/map/terrain_100.png");
	*/

	heightmap[0] = new Texture("textures/gtav-heightmap-satellite-huge.jpg");
	map_texture[0] = new Texture("textures/gtav-map-satellite-huge.jpg");
	m_shader = std::make_unique<omen::Shader>("shaders/terrain.glsl");
		
	int index = 0;
	GLfloat offset = 10.0f;
	for (GLint y = -10; y < 10; y += 2)
	{
		for (GLint x = -10; x < 10; x += 2)
		{
			glm::vec2 translation;
			translation.x = (GLfloat)x + offset;
			translation.y = (GLfloat)y + offset;
			translations[index++] = translation;
		}
	}
}


TerrainRenderer::~TerrainRenderer()
{
}


void TerrainRenderer::render()
{
	// Use shader
	m_shader->use();
	check_gl_error();
	// Get matrices
	glm::mat4 viewMatrix = Engine::instance()->camera()->view();
	check_gl_error();
	glm::mat4 viewprojMatrix = Engine::instance()->camera()->viewProjection();
	check_gl_error();
	glm::mat4 modelMatrix = entity()->getComponent<Transform>()->tr();
	check_gl_error();
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	check_gl_error();
	glm::mat4 MVP = viewprojMatrix * modelMatrix;
	check_gl_error();
	glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(modelViewMatrix));
	check_gl_error();
	glm::vec3 viewPos = Engine::instance()->camera()->position();
	check_gl_error();
	glm::vec4 diffuseColor = meshController()->mesh()->material()->const_diffuseColor();
	check_gl_error();
	m_shader->setUniform4fv("MaterialDiffuse", 1, glm::value_ptr(diffuseColor));
	check_gl_error();

	// Set matrix uniforms
	m_shader->setUniformMatrix4fv("ViewMatrix", 1, glm::value_ptr(viewMatrix), false);
	check_gl_error();
	m_shader->setUniformMatrix4fv("ViewProjMatrix", 1, glm::value_ptr(modelMatrix), false);
	check_gl_error();
	m_shader->setUniformMatrix4fv("ModelMatrix", 1, glm::value_ptr(normalMatrix), false);
	check_gl_error();
	m_shader->setUniformMatrix4fv("ModelViewMatrix", 1, glm::value_ptr(modelViewMatrix), false);
	check_gl_error();
	m_shader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(MVP), false);
	check_gl_error();
	m_shader->setUniformMatrix3fv("NormalMatrix", 1, glm::value_ptr(normalMatrix), false);
	check_gl_error();
	m_shader->setUniform2fv("offsets", 100, (float*)translations);
	check_gl_error();
	// Set other uniforms
	m_shader->setUniform3fv("ViewPos", 1, glm::value_ptr(viewPos));
	check_gl_error();
	glBindVertexArray(m_vao);
	check_gl_error();
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	check_gl_error();
	glEnableVertexAttribArray(VERTEX_ATTRIB_POS);
	check_gl_error();

	if (m_vbo_texture != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texture);
		glEnableVertexAttribArray(VERTEX_ATTRIB_TCOORD);
	}
	check_gl_error();
	if (m_vbo_normals != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
		glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL);
	}
	check_gl_error();
	if (m_vbo_tangents != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_tangents);
		glEnableVertexAttribArray(VERTEX_ATTRIB_TANGENT);
	}
	check_gl_error();
	if (m_vbo_bitangents != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bitangents);
		glEnableVertexAttribArray(VERTEX_ATTRIB_BITANGENT);
	}
	check_gl_error();
	// draw points 0-3 from the currently bound VAO with current in-use shader

	{
//		m_shader->setUniform1i("HasTexture", 0);
	}

	/*for (int i = 0; i < 100; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::ostringstream ostr;
		ostr << "heightmap[" << i << "]";
		glUniform1i(m_shader->getUniformLocation(ostr.str()), 1 + i);
		heightmap[i]->bind();
	}
	check_gl_error();
	for (int i = 0; i < 100; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + 99 + i);
		check_gl_error();
		std::ostringstream ostr;
		ostr << "texturemap[" << i << "]";
		glUniform1i(m_shader->getUniformLocation(ostr.str()), 100+i);
		check_gl_error();
		map_texture[i]->bind();
		check_gl_error();
	}*/
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(m_shader->getUniformLocation("heightmap"), 0);
	heightmap[0]->bind();

	glActiveTexture(GL_TEXTURE0+1);
	glUniform1i(m_shader->getUniformLocation("texturemap"), 1);
	map_texture[0]->bind();

	m_shader->setUniform1f("TessLevelInner", 1.0);
	m_shader->setUniform1f("TessLevelOuter", 1.0);
	check_gl_error();
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	check_gl_error();
	glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT, GL_LINE);
	//glPolygonMode(GL_BACK, GL_LINE);
	if (m_indexBuffer != 0)
		glDrawElementsInstanced(GL_PATCHES, m_indexBufferSize, GL_UNSIGNED_INT, (void*)0,100);
	check_gl_error();
	glPolygonMode(GL_FRONT, GL_FILL);
}