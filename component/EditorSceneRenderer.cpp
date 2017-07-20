#include "EditorSceneRenderer.h"
#include "../Entity.h"
#include "../Engine.h"
#include "../component/MeshRenderer.h"
#include "../GL_error.h"
#include <glm/gtc/type_ptr.hpp>
#include "../ui/View.h"
#include "../Shader.h"

omen::ecs::EditorSceneRenderer::EditorSceneRenderer() :
	Renderer()
{
}

void omen::ecs::EditorSceneRenderer::onAttach(Entity* e) 
{
	
}

void omen::ecs::EditorSceneRenderer::onDetach(Entity* e) 
{

}

void omen::ecs::EditorSceneRenderer::render(omen::Shader* shader) const
{
	Engine* engine = Engine::instance();
	Scene* scene = engine->scene();
	const std::vector<std::unique_ptr<Entity>>& entities = scene->entities();
	for (const auto& e : entities)
	{
		const MeshRenderer* c = e->getComponent_const<MeshRenderer>();
		if(c)
			c->render();
	}
}