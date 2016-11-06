//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "GraphicsSystem.h"
#include "../component/Renderable.h"

using namespace omen;
using namespace ecs;

void GraphicsSystem::render() {
    for(auto c : m_components){
        Renderable* r = static_cast<Renderable*>(c);
		if(r != nullptr)
			r->render();
    }
}
