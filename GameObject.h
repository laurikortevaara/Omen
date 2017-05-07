//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_GAMEOBJECT_H
#define OMEN_GAMEOBJECT_H


#include <vector>
#include "Entity.h"
#include "component/Transform.h"
#include "BoundingBox.h"

namespace omen {
	namespace ecs {

		class GameObject : public ecs::Entity {
			std::vector< std::unique_ptr<ecs::Component> > m_components;
			BoundingBox m_bounding_box;
		public:
			GameObject(const std::string &name);

			void addCompnent(std::unique_ptr<ecs::Component> component);
			void removeComponent(ecs::Component *component);

			void updateBounds();
			BoundingBox getBounds();
		};
	}
} // namespace omen

#endif //OMEN_GAMEOBJECT_H
