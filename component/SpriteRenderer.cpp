#include "Sprite.h"
#include "SpriteRenderer.h"
#include "../Entity.h"
#include "../Engine.h"
#include "../GL_error.h"
#include <glm/gtc/type_ptr.hpp>

omen::ecs::SpriteRenderer::SpriteRenderer(std::unique_ptr<omen::ecs::Sprite> sprite) : 
	Renderer(),
	m_sprite(std::move(sprite))
{
	m_sprite->setRenderer(this);
}

void omen::ecs::SpriteRenderer::onAttach(Entity* e) {

}

void omen::ecs::SpriteRenderer::onDetach(Entity* e) {

}

void omen::ecs::SpriteRenderer::render(Shader* shader)
{
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	glViewport(0, 0, w, h);
		
	m_sprite->render();
}


glm::vec2 omen::ecs::SpriteRenderer::pivot() const
{
	return m_sprite->pivot();
}

void omen::ecs::SpriteRenderer::setPivot(const glm::vec2& pivot)
{
	m_sprite->setPivot(pivot);
}