//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "GraphicsSystem.h"
#include "../component/Renderer.h"

using namespace omen;
using namespace ecs;

void GraphicsSystem::render() {
    for(auto c : m_components){
        Renderer* r = static_cast<Renderer*>(c);
		if(r != nullptr)
			r->render();
    }
}
