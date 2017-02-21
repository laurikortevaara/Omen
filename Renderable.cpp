//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#include "Renderable.h"

using namespace omen;

Renderable::Renderable(glm::vec2 pos, float width, float height, float depth) :
	Object("Renderable"),
    m_texture(nullptr), m_width(width), m_height(height), m_pos(0.0,0.0) 
{
	m_pos = pos;
}

Renderable::~Renderable() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_vbo_texcoord);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);
    if (m_shader != nullptr)
        delete m_shader;
    if (m_shader != nullptr)
        delete m_texture;
}

void Renderable::setPos(const glm::vec2& pos)
{
	m_pos = pos;
}