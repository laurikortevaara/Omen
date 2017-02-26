//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#ifndef OMEN_DRAGGABLE_H
#define OMEN_DRAGGABLE_H

#include "Component.h"
#include <glm/glm.hpp>

namespace omen {
    namespace ecs {
        class Draggable : public ecs::Component {
        protected:
            virtual void onAttach(ecs::Entity* e);
            virtual void onDetach(ecs::Entity* e);
        public:
            typedef omen::Signal<std::function<void(Entity*, glm::vec2)> > Clicked_t;
            Clicked_t signal_clicked;

			typedef omen::Signal<std::function<void(float value)> > Dragged_t;
			Dragged_t signal_dragged;

        public:
			Draggable(const glm::vec2& groovePos = { 0.0f,0.0f }, const glm::vec2& grooveSize = { -1.0f,-1.0f });
            virtual ~Draggable();

			void setPos(float pos);
			float pos() const;
        private:
			glm::vec3 m_groovePos;
			glm::vec3 m_grooveSize;
			glm::vec2 m_deltaPos;
            glm::vec2 m_cursorPos;
			bool m_is_pressed;
        };
    }
}


#endif //OMEN_DRAGGABLE_H
