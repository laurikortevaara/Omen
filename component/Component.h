//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_COMPONENT_H
#define OMEN_COMPONENT_H

#include <functional>
#include "../Signal.h"
#include <memory>
#include "../Object.h"

namespace omen {
    namespace ecs {
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

            ComponentAttached_t signal_compnent_attached;
            ComponentDetached_t signal_component_detached;
			Component_Destructed_t signal_component_destructed;


        public:
			Component();

            virtual ~Component();

            virtual void update(double time, double deltaTime) { };

            bool enabled() const { return m_isEnabled; }

            void setEnabled(bool enabled) { m_isEnabled = enabled; }

            void setEntity(omen::ecs::Entity *pEntity);
            Entity* entity() const;
        };
    } // namespace ecs
} // namespace omen


#endif //OMEN_COMPONENT_H
