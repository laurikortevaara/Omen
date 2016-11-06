//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "Entity.h"
#include "Component/Component.h"
#include "Component/Transform.h"

using namespace omen::ecs;

bool Entity::addChild(std::shared_ptr<Entity> e) {if(std::find(m_children.begin(), m_children.end(), e)==m_children.end()){m_children.push_back(e);e->m_parent=this;return true;}else return false;}
bool Entity::removeChild(std::shared_ptr<Entity> e) { std::vector<std::shared_ptr<Entity>>::iterator iter = std::find(m_children.begin(), m_children.end(), e); if(iter!=m_children.end()){m_children.erase(iter);e->m_parent=nullptr;return true;}else return false;}

bool Entity::addComponent(std::shared_ptr<Component> c) {
    if(std::find(m_components.begin(), m_components.end(), c)==m_components.end()){
        m_components.push_back(c);c->setEntity(this);
        return true;
    }
    else
        return false;
}
bool Entity::removeComponent(std::shared_ptr<Component> c) {
    std::vector<std::shared_ptr<Component>>::iterator iter = std::find(m_components.begin(), m_components.end(), c);
    if(iter!=m_components.end()){m_components.erase(iter);c->setEntity(nullptr);
        return true;
    }
    else
        return false;
}

Entity::Entity(const std::string &name):
        Object(name), m_parent(nullptr) {}
