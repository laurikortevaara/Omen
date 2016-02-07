//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#ifndef OMEN_CLICKABLE_H
#define OMEN_CLICKABLE_H

#include "Component.h"
#include <glm/glm.hpp>

namespace omen {
    namespace ecs {
        class Clickable : public ecs::Component {
        protected:
            virtual void onAttach(ecs::Entity* e);
            virtual void onDetach(ecs::Entity* e);
        public:
            typedef omen::Signal<std::function<void(Entity*, glm::vec2)> > EntityClicked_t;

            EntityClicked_t signal_entity_clicked;
        public:
            Clickable(Entity* e);
            virtual ~Clickable();
        private:
            glm::vec2 m_cursorPos;
        };
    }
}


#endif //OMEN_CLICKABLE_H