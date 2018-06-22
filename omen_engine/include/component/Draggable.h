//
// Created by Lauri Kortevaara on 04/02/16.
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

			typedef omen::Signal<std::function<void(omen::ecs::Draggable* draggable, float value)> > DragStarted_t;
			static DragStarted_t signal_drag_started;

			typedef omen::Signal<std::function<void(omen::ecs::Draggable* draggable, float value)> > DragStopped_t;
			static DragStopped_t signal_drag_stopped;

			typedef omen::Signal<std::function<void(float value)> > Dragged_t;
			Dragged_t signal_dragged;

			typedef omen::Signal<std::function<void(glm::vec2 value)> > DraggedXY_t;
			DraggedXY_t signal_draggedXY;

        public:
			Draggable(const glm::vec2& groovePos = { 0.0f,0.0f }, const glm::vec2& grooveSize = { -1.0f,-1.0f });
            virtual ~Draggable();

			void setPos(float pos);
			float pos() const;

			void setGroovePos(glm::vec3 pos) { m_groovePos = pos; }
			void setGrooveSize(glm::vec3 size) { m_grooveSize = size; }
			glm::vec3 groovePos() const { return m_groovePos; }
			glm::vec3 grooveSize() const { return m_grooveSize; }
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
