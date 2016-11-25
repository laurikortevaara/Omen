//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "Component.h"

using namespace omen::ecs;

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
