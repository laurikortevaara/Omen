//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "GameObject.h"
#include <algorithm>

using namespace omen;

GameObject::GameObject(const std::string &name) :
        Entity(name){
    m_tr = new Transform();
    addCompnent(m_tr);
}

void GameObject::addCompnent(ecs::Component *component) {
    m_components.push_back(component);
}

void GameObject::removeComponent(ecs::Component *component) {
    std::vector<ecs::Component *>::iterator iter = std::find(m_components.begin(), m_components.end(), component);
    if (iter != m_components.end())
        m_components.erase(iter);
}
