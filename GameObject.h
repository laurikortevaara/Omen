//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_GAMEOBJECT_H
#define OMEN_GAMEOBJECT_H


#include <vector>
#include "Entity.h"
#include "component/Transform.h"

namespace omen {
    class GameObject : public ecs::Entity {
        Transform* m_tr;
        std::vector<ecs::Component *> m_components;
    public:
        GameObject(const std::string &name);

        void addCompnent(ecs::Component *component);
        void removeComponent(ecs::Component *component);

    };
} // namespace omen

#endif //OMEN_GAMEOBJECT_H
