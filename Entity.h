//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_ENTITY_H
#define OMEN_ENTITY_H

#include <string>
#include <stdint.h>
#include <vector>

namespace omen {
    namespace ecs {
        class Component;
        class Entity {
            Entity* m_parent;
            std::vector<Entity*> m_children;
            std::vector<Component*> m_components;
            static uint64_t id_counter;
        protected:
            uint64_t m_id;
            std::string m_name;

            Entity(const std::string &name);

            uint64_t id() const { return m_id; }

            Entity* parent() {return m_parent;}
            std::vector<Entity*>& children() {return m_children;}
            bool addChild(Entity* e);
            bool removeChild(Entity* e);

            bool removeComponent(Component* c);

        public:
            template<class type>
            type *getComponent(const std::string &component_name = "") {
                for (auto c : m_components)
                    if (dynamic_cast<type *>(c) != nullptr)
                        return dynamic_cast<type *>(c);
                return nullptr;
            }

            bool addComponent(Component* c);
        };
    } // namespace ecs
} // namespace omen




#endif //OMEN_ENTITY_H
