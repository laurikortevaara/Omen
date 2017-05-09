#include "Renderer.h"

using namespace omen;
using namespace ecs;

static GLint polymode[2];

Renderer::Renderer() : 
	Component(),
	m_vao(0),
	m_vbo(0),
	m_vbo_texture(0),
	m_vbo_normals(0),
	m_vbo_tangents(0),
	m_vbo_bitangents(0),

	m_indexBuffer(0),
	m_indexBufferSize(0)
{
	GraphicsSystem *gs = omen::Engine::instance()->findSystem<GraphicsSystem>();
	assert(gs != nullptr);
	gs->addComponent(this);

	OpenVRSystem *vs = omen::Engine::instance()->findSystem<OpenVRSystem>();
	assert(vs != nullptr);
	vs->addComponent(this);
	int a = 1;
};

void Renderer::storePolygonMode()
{
	glGetIntegerv(GL_POLYGON_MODE, polymode);
}

void Renderer::polygonModeFill()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::polygonModeWireframe()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
}

void Renderer::restorePolygonMode()
{
	glPolygonMode(GL_FRONT, polymode[0]);
	glPolygonMode(GL_BACK, polymode[1]);
}