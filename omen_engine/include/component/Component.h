//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_COMPONENT_H
#define OMEN_COMPONENT_H

#include "../Signal.h"
#include "../Object.h"

#include <functional>
#include <memory>

namespace omen {
    namespace ecs {
		class System;
        class Entity;
        class Component : public omen::Object {
        protected:
            Entity *m_entity = nullptr;
            bool m_isEnabled = false;

            virtual void onAttach(ecs::Entity* e) = 0;
            virtual void onDetach(ecs::Entity* e) = 0;

        public:
            typedef omen::Signal<std::function<void(Entity*, Component*)> > ComponentAttached_t;
            typedef omen::Signal<std::function<void(Entity*, Component*)> > ComponentDetached_t;
			typedef omen::Signal<std::function<void(Component*) > > Component_Destructed_t;

            ComponentAttached_t signal_component_attached;
            ComponentDetached_t signal_component_detached;
			Component_Destructed_t signal_component_destructed;

			void attachToSystem(System* system);
			void detachFromSystem();

        public:
			Component();

            virtual ~Component();

            virtual void update(double time, double deltaTime) { };

            bool enabled() const { return m_isEnabled; }

            void setEnabled(bool enabled) { m_isEnabled = enabled; }

            void setEntity(omen::ecs::Entity *pEntity);
            Entity* entity() const;
			System* m_system;
        };
    } // namespace ecs
} // namespace omen


#endif //OMEN_COMPONENT_H
