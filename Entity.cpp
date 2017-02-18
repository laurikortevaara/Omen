//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "Entity.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Engine.h"

using namespace omen::ecs;

bool Entity::addChild(std::unique_ptr<Entity> e) {
	if(std::find(m_children.begin(), m_children.end(), e)==m_children.end())
	{
		e->m_parent = this;
		Engine::instance()->scene()->signal_entity_added.notify(e.get());
		m_children.push_back(std::move(e));		
		return true;
	}
	else 
		return false;
}

bool Entity::removeChild(std::unique_ptr<Entity> e) { 
	std::vector<std::unique_ptr<Entity>>::iterator iter = 
		std::find(m_children.begin(), m_children.end(), e); 
	if(iter!=m_children.end())
	{
		m_children.erase(iter);
		e->m_parent=nullptr;
		return true;
	}
	else 
		return false;
}

bool Entity::addComponent(std::unique_ptr<Component> c) {
    if(std::find(m_components.begin(), m_components.end(), c)==m_components.end()){
		c->setEntity(this);
		m_components.push_back(std::move(c));
        return true;
    }
    else
        return false;
}
bool Entity::removeComponent(std::unique_ptr<Component> c) {
    std::vector<std::unique_ptr<Component>>::iterator iter = std::find(m_components.begin(), m_components.end(), c);
    if(iter!=m_components.end()){m_components.erase(iter);c->setEntity(nullptr);
        return true;
    }
    else
        return false;
}

Entity::Entity(const std::string &name):
        Object(name), m_parent(nullptr), m_layer(0) {}

/*
template<class type>
const type* Entity::getComponent(const std::string &component_name) {
	for (auto& c : m_components)
		if (std::dynamic_pointer_cast<type*>(c.get()) != nullptr)
			return std::dynamic_pointer_cast<type*>(c.get());
	return nullptr;
}*/

Entity* Entity::findChild(const std::string& name) {
	for (const auto& child : m_children) {
		if (child->name() == name)
			return child.get();
		Entity* matchingChild = child->findChild(name);
		if (matchingChild != nullptr)
			return matchingChild;
	}
	return nullptr;
}

Entity const* Entity::findChild_const(const std::string& name) const {
	for (const auto& child : m_children) {
		if (child->name() == name)
			return child.get();
		Entity const* matchingChild = child->findChild_const(name);
		if (matchingChild != nullptr)
			return matchingChild;
	}
	return nullptr;
}