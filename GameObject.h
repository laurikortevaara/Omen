//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_GAMEOBJECT_H
#define OMEN_GAMEOBJECT_H


#include <vector>
#include "Entity.h"
#include "Transform.h"
#include "Component.h"

namespace Omen {
    class GameObject : public ecs::Entity {
        Transform m_tr;
        std::vector<Component *> m_components;
    public:
        GameObject(const std::string &name);

        void addCompnent(Component *component);

        void removeComponent(Component *component);

    };
} // namespace Omen

#endif //OMEN_GAMEOBJECT_H
