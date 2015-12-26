//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "GameObject.h"

using namespace Omen;

GameObject::GameObject(const std::string &name) :
        Entity(name) {

}

void GameObject::addCompnent(Component *component) {
    m_components.push_back(component);
}

void GameObject::removeComponent(Component *component) {
    std::vector<Component *>::iterator iter = std::find(m_components.begin(), m_components.end(), component);
    if (iter != m_components.end())
        m_components.erase(iter);
}
