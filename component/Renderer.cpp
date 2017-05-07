#include "Renderer.h"

using namespace omen;
using namespace ecs;


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