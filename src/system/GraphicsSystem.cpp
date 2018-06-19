//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "system/GraphicsSystem.h"

#include "Shader.h"

#include "component/Renderer.h"
#include "component/MeshRenderer.h"

#include "ui/View.h"


using namespace omen;
using namespace ecs;

long long GraphicsSystem::drawElementsCount = 0;
long long GraphicsSystem::drawArraysCount = 0;
long long GraphicsSystem::drawElementsInstancedCount = 0;


void GraphicsSystem::render(omen::Shader* shader, int layer) {
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	//Engine::instance()->setViewport(0, 0, w, h);

	std::vector<Renderer*> renderers;

	//glClear(GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//glDisable(GL_BLEND);*/
	//glDepthFunc()
    for(auto c : components()){
		Renderer* r = dynamic_cast<Renderer*>(c);
		MeshRenderer* mr = dynamic_cast<MeshRenderer*>(c);

		if (layer != -1 && layer != c->entity()->layer())
			continue;

		const char* e = c->entity()->name().c_str();
		if (strcmp(e, "LightbulbHD") == 0 && shader != nullptr)
			continue;
		
       
		// Don't render anything if no mesh renderer and given shader
		if (mr == nullptr && shader != nullptr)
			continue;
		if (mr != nullptr && shader != nullptr)
			mr->renderShadows(shader);
		else
			if (r != nullptr && shader == nullptr)
				//renderers.push_back(r);
			{
				glDisable(GL_DEPTH_TEST);
				// If this is a 2D element set the clipping 
				if (mr == nullptr)
				{
					Entity* entity = c->entity();
					ui::View* v = dynamic_cast<ui::View*>(entity);

					glm::vec2 pos = entity->pos2D();
					glm::vec2 localPos = entity->localPos2D();

					glm::vec3 bmin, bmax;
					entity->tr()->getClippedBounds(bmin, bmax);
					bmax.x -= v->margins()[0]+v->margins()[2];
					bmax.y -= v->margins()[1]+v->margins()[3];
					//glScissor(bmin.x, bmin.y, bmax.x-bmin.x, bmax.y-bmin.y);
					float width = entity->width(); // bmax.x - bmin.x;
					float height = entity->height(); // bmax.y - bmin.y;
					
					//std::cout << "Setting scissor to: " << width << " , " << height << "\n";
					//glScissor(pos.x + bmin.x, Engine::instance()->window()->height() - height - pos.y-150, width, height);
					glViewport((GLint)(pos.x + bmin.x),(GLint)( Engine::instance()->window()->height() - height - pos.y), (GLsizei)width, (GLsizei)height);
					glScissor ((GLint)(pos.x + bmin.x),(GLint)( Engine::instance()->window()->height() - height - pos.y), (GLsizei)width, (GLsizei)height);
				}
				r->render(shader);
				glEnable(GL_DEPTH_TEST);
			}
    }
	/*std::sort(renderers.begin(), renderers.end(), [](Renderer* r1, Renderer* r2)-> bool {
		float z1 = r1->entity() != nullptr && r1->entity()->getComponent<Transform>() != nullptr ? r1->entity()->getComponent<Transform>()->pos().z : 0.0f;
		float z2 = r2->entity() != nullptr && r2->entity()->getComponent<Transform>() != nullptr ? r2->entity()->getComponent<Transform>()->pos().z : 0.0f;

		return z1 > z2; });*/
	
	/*for(auto r : renderers)
		r->render();*/
}

void GraphicsSystem::addComponent(Component* component)
{
	if (component->entity() != nullptr)
	{
		component->entity()->signal_layer_changed.connect(this, [&](ecs::Entity* e) {
			std::sort(components().begin(), components().end(), [&](omen::ecs::Component* left, omen::ecs::Component* right) {
				return left->entity()->layer() < right->entity()->layer();
			});
		});
	}
	component->signal_component_attached.connect(this, [&](Entity*e, Component*c) {
		e->signal_layer_changed.connect(this, [&](ecs::Entity* e) {
			std::sort(components().begin(), components().end(), [&](omen::ecs::Component* left, omen::ecs::Component* right) {
				return left->entity()->layer() < right->entity()->layer();
			});
		});
	});
	System::addComponent(component);
}