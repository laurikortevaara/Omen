//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_COMPONENT_H
#define OMEN_COMPONENT_H

#include <functional>
#include "../Signal.h"
#include <memory>

namespace omen {
    namespace ecs {
        class Entity;
        class Component : public std::enable_shared_from_this<Component> {
        protected:
            Entity *m_entity = nullptr;
            bool m_isEnabled = false;

            virtual void onAttach(ecs::Entity* e) = 0;
            virtual void onDetach(ecs::Entity* e) = 0;

        public:
            typedef omen::Signal<std::function<void(Entity*, Component*)> > ComponentAttached_t;
            typedef omen::Signal<std::function<void(Entity*, Component*)> > ComponentDetached_t;

            ComponentAttached_t signal_compnent_attached;
            ComponentDetached_t signal_component_detached;

        public:
            Component() : m_isEnabled(true) { };

            virtual ~Component() { };

            virtual void update(double time, double deltaTime) { };

            bool enabled() const { return m_isEnabled; }

            void setEnabled(bool enabled) { m_isEnabled = enabled; }

            void setEntity(omen::ecs::Entity *pEntity);
            Entity* entity() const;
        };
    } // namespace ecs
} // namespace omen


#endif //OMEN_COMPONENT_H
