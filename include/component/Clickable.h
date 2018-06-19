//
// Created by Lauri Kortevaara on 04/02/16.
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

			bool cursorInsideEntity();
        public:
			typedef omen::Signal<std::function<void(Entity*, glm::vec2, int button)> > EntityMouseClicked_t;
			EntityMouseClicked_t signal_entity_mouse_clicked;

			typedef omen::Signal<std::function<void(Entity*, glm::vec2, int button)> > EntityMousePressed_t;
			EntityMousePressed_t signal_entity_mouse_pressed;

            typedef omen::Signal<std::function<void(Entity*, glm::vec2, int button)> > EntityMouseReleased_t;
			EntityMouseReleased_t signal_entity_mouse_released;

        public:
            Clickable();
            virtual ~Clickable();
        private:
			glm::vec2 m_deltaPos;
            glm::vec2 m_cursorPos;
			bool m_is_pressed;
        };
    }
}


#endif //OMEN_CLICKABLE_H
