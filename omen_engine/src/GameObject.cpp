//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "GameObject.h"
#include <algorithm>
#include "component/Transform.h"
#include "MeshProvider.h"
#include "component/MeshRenderer.h"
#include "component/MeshController.h"

using namespace omen;
using namespace ecs;

GameObject::GameObject(const std::string &name) :
	Entity(name) {
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

void GameObject::updateBounds()
{
	if (getComponent<MeshRenderer>() != nullptr)
	{
		MeshRenderer* mr = getComponent<MeshRenderer>();
		if(mr->meshController() != nullptr && mr->meshController()->mesh() != nullptr)
		{
			mr->meshController()->mesh()->calcBoundingBox();
			this->m_bounding_box.set(mr->meshController()->mesh()->boundingBox()->Min(), mr->meshController()->mesh()->boundingBox()->Max());
		}
	}
	
	for (auto& child : this->children())
	{
		GameObject* childGM = dynamic_cast<GameObject*>(child.get());
		if (childGM) 
		{
			childGM->updateBounds();
			BoundingBox bounds = childGM->getBounds();
			glm::vec3 m1 = bounds.Max();
			glm::vec3 m2 = m_bounding_box.Max();
			glm::vec3 m3 = bounds.Min();
			glm::vec3 m4 = m_bounding_box.Min();
			m_bounding_box.set(glm::length(m3) > glm::length(m4) ? m3 : m4,  glm::length(m1) > glm::length(m2) ? m1 : m2 );
		}
	}
}

BoundingBox GameObject::getBounds()
{
	return m_bounding_box;
}

GameObject::~GameObject()
{
	std::cout << "Killing Gameobject";
}