//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "GameObject.h"
#include <algorithm>
#include "component/Transform.h"

using namespace omen;
using namespace ecs;

GameObject::GameObject(const std::string &name) :
        Entity(name){
    std::unique_ptr<Transform> tr = std::make_unique<Transform>();
	m_tr = tr.get();
    addCompnent(std::move(tr));
}

void GameObject::addCompnent(std::unique_ptr<omen::ecs::Component> component) {
	Entity::addComponent(std::move(component));
}

void GameObject::removeComponent(ecs::Component *component) {
    std::vector< std::unique_ptr<ecs::Component> >::iterator iter = 
		std::find_if(m_components.begin(), m_components.end(), 
			[component](std::unique_ptr<ecs::Component> const& a)->bool { return a.get() == component; });
    if (iter != m_components.end())
        m_components.erase(iter);
}
