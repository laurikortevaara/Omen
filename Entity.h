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
			std::vector<std::unique_ptr<Entity>> m_children;
			std::vector<std::unique_ptr<Component>> m_components;

		protected:
			Entity(const std::string &name);
			Entity* parent() { return m_parent; }

            const std::vector<std::unique_ptr<Entity>>& children() {return m_children;}
            bool addChild(std::unique_ptr<Entity> e);
            bool removeChild(std::unique_ptr<Entity> e);

            bool removeComponent(std::unique_ptr<Component> c);

        public:
            template<class type>
			type* getComponent(const std::string &component_name = "") {
				for (auto& c : m_components)
					if (c.get() != nullptr)
						return dynamic_cast<type*>(c.get());
				return nullptr;
			}

            bool addComponent(std::unique_ptr<Component> c);

			virtual ~Entity() {};
        };
    } // namespace ecs
} // namespace omen

#endif //OMEN_ENTITY_H
