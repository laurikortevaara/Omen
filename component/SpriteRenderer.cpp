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
}

void omen::ecs::SpriteRenderer::onAttach(Entity* e) {

}

void omen::ecs::SpriteRenderer::onDetach(Entity* e) {

}

void omen::ecs::SpriteRenderer::render()
{
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	glViewport(0, 0, w, h);
		
	m_sprite->render();
}
