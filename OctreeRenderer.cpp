#define NOMINMAX
#include "OctreeRenderer.h"
#include "Entity.h"
#include "MeshProvider.h"
#include "Omen_OpenGL.h"
#include "GL_error.h"
#include "GameObject.h"
#include "component/MeshController.h"
#include "Engine.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

using namespace omen;
using namespace ecs;

void OctreeRenderer::initializeRenderer()
{
	m_shader = new Shader("shaders/octree_shader.glsl");

	std::list< std::unique_ptr<omen::ecs::GameObject> > listobj = MeshProvider::loadObject("models/cube.fbx");

	for (const auto& obj : listobj) {
		MeshController* mc = obj->getComponent<MeshController>();
		mesh = mc->mesh().get();
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
		std::vector<omen::Mesh::Vertex> &verts = mesh->vertices();
		std::vector<glm::vec3> vertices;
		for (auto& v : verts) vertices.push_back(v.pos);
		check_gl_error();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
		check_gl_error();
		glVertexAttribPointer(VERTEX_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, 0);
		check_gl_error();
		glDisableVertexAttribArray(VERTEX_ATTRIB_POS);

		// Check if we have index buffer
		m_indexBufferSize = mesh->vertexIndices().size();
		if (m_indexBufferSize > 0) {
			glGenBuffers(1, &m_indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferSize * sizeof(unsigned int), mesh->vertexIndices().data(), GL_STATIC_DRAW);

			std::vector<glm::vec3> barycoords;
			int index = 0;
			for (size_t i = 0; i < mesh->vertexIndices().size(); i++) {
				int index = i % 3; // mesh->vertexIndices()[i] % 3;
				barycoords.push_back({ index == 0, index == 1?1000.0f:0.0f, index == 2 });
			}
			glGenBuffers(1, &m_vbo_barycentric);
			glEnableVertexAttribArray(VERTEX_ATTRIB_BARYCENTRIC);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_barycentric);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*barycoords.size(), barycoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(VERTEX_ATTRIB_BARYCENTRIC, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glDisableVertexAttribArray(VERTEX_ATTRIB_BARYCENTRIC);
		}
		// Create VBO for texture
		std::vector<glm::vec2> uvs = mesh->uv();
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
		}

		// Create VBO for normals
		std::vector<glm::vec3> normals = mesh->normals();
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
		std::vector<glm::vec3> tangents = mesh->tangents();
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
		std::vector<glm::vec3> bitangents = mesh->bitangents();
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
	}
}

void OctreeRenderer::onAttach(ecs::Entity * e)
{
}

void OctreeRenderer::onDetach(ecs::Entity * e)
{
}

void OctreeRenderer::renderNode(Octree::Node* node, int level)
{
	if (Engine::instance()->meshRendererEnabled())
		return;
	glm::mat4 model;
	model = glm::translate(model, node->pos);
	//model = glm::scale(model, glm::vec3(node->size));
	Camera* camera = Engine::instance()->camera();
	glm::mat4 viewproj = camera->viewProjection()*model;
	m_shader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(viewproj), false);
	m_shader->setUniformMatrix4fv("ViewMatrix", 1, glm::value_ptr(camera->view()), false);

	static int max_level = 0;
	max_level = std::max(level, max_level);
	
	if (level == 0) {
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

		if (m_vbo_barycentric != 0) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_barycentric);
			glEnableVertexAttribArray(VERTEX_ATTRIB_BARYCENTRIC);
		}

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		if (m_indexBuffer != 0)
			glDrawElementsInstanced(GL_TRIANGLES, m_indexBufferSize, GL_UNSIGNED_INT, (void*)0, 1);
	}

	for(auto& child : node->children)
		if(child)
			renderNode(child, level + 1);
}

void OctreeRenderer::render(omen::Shader * shader)
{
	if (!m_octree->rootNode)
		return;
	m_shader->use();
	
	renderNode(m_octree->rootNode, 0);
}
