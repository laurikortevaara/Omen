//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#include "Renderable.h"
#include "../system/GraphicsSystem.h"
#include "../Engine.h"

using namespace omen;
using namespace ecs;

Renderable::Renderable(glm::vec3 pos, float width, float height, float depth) :
        m_texture(nullptr), m_width(width), m_height(height), m_pos(pos) {
    GraphicsSystem *gs = omen::Engine::instance()->findSystem<GraphicsSystem>();
    assert(gs != nullptr);
    gs->add(this);
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

void Renderable::onAttach(Entity *e) {
    m_entity = e;
}

void Renderable::onDetach(ecs::Entity *e) {

}
