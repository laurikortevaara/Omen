//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "component/Component.h"
#include "system/System.h"

using namespace omen::ecs;

Component::Component() : 
	omen::Object(typeid(this).name()), m_isEnabled(true), m_system(nullptr)
{ 
};

void Component::setEntity(omen::ecs::Entity *pEntity) {
	if (pEntity != nullptr)
	{
		onAttach(pEntity);
		signal_component_attached.notify(pEntity, this);
	}
	else
	{
		onDetach(pEntity);
		signal_component_detached.notify(pEntity, this);
	}
    m_entity = pEntity;
	
}

Entity *Component::entity() const {
    return m_entity;
}

Component::~Component() 
{
 	signal_component_destructed.notify(this);
	if(m_system != nullptr)
		m_system->detachFromSystem(this);
	int a = 1;
};

void Component::attachToSystem(System* system)
{
	m_system = system;
}

void Component::detachFromSystem()
{
	if (m_system != nullptr)
	{
		m_system->detachFromSystem(this);
	}
}