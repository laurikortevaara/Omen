//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_GAMEOBJECT_H
#define OMEN_GAMEOBJECT_H


#include <vector>
#include "Entity.h"
#include "component/Transform.h"

namespace omen {
	namespace ecs {

		class GameObject : public ecs::Entity {
			Transform* m_tr;
			std::vector< std::unique_ptr<ecs::Component> > m_components;
		public:
			GameObject(const std::string &name);

			void addCompnent(std::unique_ptr<ecs::Component> component);
			void removeComponent(ecs::Component *component);

			Transform* transform() const { return m_tr; }

		};
	}
} // namespace omen

#endif //OMEN_GAMEOBJECT_H
