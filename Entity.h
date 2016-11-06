//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_ENTITY_H
#define OMEN_ENTITY_H

#include <string>
#include <stdint.h>
#include <vector>
#include <memory>

#include "Object.h"

namespace omen {
    namespace ecs {
        class Component;
		class Entity : public omen::Object {

			Entity* m_parent;
			std::vector<std::shared_ptr<Entity>> m_children;
			std::vector<std::shared_ptr<Component>> m_components;

		protected:
			Entity(const std::string &name);
			Entity* parent() { return m_parent; }

			virtual ~Entity() {};

            std::vector<std::shared_ptr<Entity>>& children() {return m_children;}
            bool addChild(std::shared_ptr<Entity> e);
            bool removeChild(std::shared_ptr<Entity> e);

            bool removeComponent(std::shared_ptr<Component> c);

        public:
            template<class type>
            std::shared_ptr<type> getComponent(const std::string &component_name = "") {
                for (auto c : m_components)
                    if (std::dynamic_pointer_cast<type>(c) != nullptr)
                        return std::dynamic_pointer_cast<type>(c);
                return nullptr;
            }

            bool addComponent(std::shared_ptr<Component> c);
        };
    } // namespace ecs
} // namespace omen

#endif //OMEN_ENTITY_H
