//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "Component.h"

using namespace omen::ecs;

Component::Component() : 
	omen::Object(typeid(this).name()), m_isEnabled(true) 
{ 
};

void Component::setEntity(omen::ecs::Entity *pEntity) {
    if(pEntity!= nullptr)
        onAttach(pEntity);
    else
        onDetach(pEntity);
    m_entity = pEntity;
}

Entity *Component::entity() const {
    return m_entity;
}

Component::~Component() 
{
	signal_component_destructed.notify(this);
	int a = 1;
};