//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "GraphicsSystem.h"
#include "../component/Renderer.h"

using namespace omen;
using namespace ecs;

void GraphicsSystem::add(Component* component)
{
	m_components.push_back(component); 
}

void GraphicsSystem::render() {
	std::vector<Renderer*> renderers;
    for(auto c : m_components){
        Renderer* r = dynamic_cast<Renderer*>(c);
		if (r != nullptr)
			renderers.push_back(r);
    }
	std::sort(renderers.begin(), renderers.end(), [](Renderer* r1, Renderer* r2)-> bool {
		float z1 = r1->entity() != nullptr && r1->entity()->getComponent<Transform>() != nullptr ? r1->entity()->getComponent<Transform>()->pos().z : 0.0f;
		float z2 = r2->entity() != nullptr && r2->entity()->getComponent<Transform>() != nullptr ? r2->entity()->getComponent<Transform>()->pos().z : 0.0f;

		return z1 > z2; });
	
	for(auto r : renderers)
		r->render();
}
