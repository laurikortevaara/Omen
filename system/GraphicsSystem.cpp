//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "GraphicsSystem.h"
#include "../component/Renderer.h"
#include "../component/MeshRenderer.h"
#include "../Shader.h"

using namespace omen;
using namespace ecs;

void GraphicsSystem::add(Component* component)
{
	m_components.push_back(component); 
}

void GraphicsSystem::render(omen::Shader* shader) {
	int w, h;
	glfwGetFramebufferSize(Engine::instance()->window()->window(), &w, &h);
	//glViewport(0, 0, w, h);

	std::vector<Renderer*> renderers;

	//glClear(GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	//glDisable(GL_BLEND);*/
	//glDepthFunc()
    for(auto c : m_components){
        Renderer* r = dynamic_cast<Renderer*>(c);
		if (r != nullptr)
			//renderers.push_back(r);
			r->render(shader);
    }
	/*std::sort(renderers.begin(), renderers.end(), [](Renderer* r1, Renderer* r2)-> bool {
		float z1 = r1->entity() != nullptr && r1->entity()->getComponent<Transform>() != nullptr ? r1->entity()->getComponent<Transform>()->pos().z : 0.0f;
		float z2 = r2->entity() != nullptr && r2->entity()->getComponent<Transform>() != nullptr ? r2->entity()->getComponent<Transform>()->pos().z : 0.0f;

		return z1 > z2; });*/
	
	/*for(auto r : renderers)
		r->render();*/
}
