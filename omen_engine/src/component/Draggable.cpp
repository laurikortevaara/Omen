//
// Created by Lauri Kortevaara on 04/02/16.
//

#include "component/Draggable.h"

#include "Entity.h"
#include "Engine.h"
#include "component/MouseInput.h"
#include "component/SpriteRenderer.h"
#include "component/Sprite.h"
#include "component/Transform.h"
#include "ui/View.h"

using namespace omen::ecs;

Draggable::DragStarted_t Draggable::signal_drag_started;
Draggable::DragStopped_t Draggable::signal_drag_stopped;

Draggable::Draggable(const glm::vec2& groovePos, const glm::vec2& grooveSize) :
	m_is_pressed(false),
	m_groovePos(glm::vec3(groovePos.x, groovePos.y, 0.0f)),
	m_grooveSize(glm::vec3(grooveSize.x, grooveSize.y, 0.0f))
{
    Engine::instance()->findComponent<MouseInput>()->
            signal_mousebutton_pressed.connect(this,[&](int button, int action, int mods, const glm::vec2& cursorPos) -> void {
        if (action != GLFW_PRESS || entity() == nullptr)
            return;
				
			omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());
			glm::vec3 bmin, bmax;
			tr->getBounds(bmin, bmax);

            if(m_cursorPos.x >= (entity()->pos().x) &&
               m_cursorPos.x <= (entity()->pos().x+bmax.x) &&
               m_cursorPos.y >= (entity()->pos().y) &&
               m_cursorPos.y <= (entity()->pos().y+bmax.y)) {
				m_deltaPos = glm::vec2(m_cursorPos.x-entity()->pos().x, m_cursorPos.y-entity()->pos().y);
                signal_clicked.notify(entity(),m_cursorPos);
				Draggable::signal_drag_started.notify(this, pos());
				m_is_pressed = true;
            
        }
    });

	Engine::instance()->findComponent<MouseInput>()->
		signal_mousebutton_released.connect(this,[&](int button, int action, int mods, const glm::vec2& cursorPos) -> void 
		{
			m_is_pressed = false;
			signal_drag_stopped.notify(this, pos());
		});

    Engine::instance()->findComponent<MouseInput>()->
            signal_cursorpos_changed.connect(this,[&](float x, float y) -> void {
        m_cursorPos = glm::vec2(x,y);
		if (m_is_pressed) {
			omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());
			glm::vec3 bmin, bmax; // Dragged view bounding volume
			glm::vec3 grooveSize;
			tr->getBounds(bmin, bmax);

			glm::vec2 newPos = { x - m_deltaPos.x - entity()->parent()->pos().x, y - m_deltaPos.y - entity()->parent()->pos().y };

			grooveSize = this->m_grooveSize;

			newPos.x = glm::clamp(newPos.x, 0.0f + groovePos.x, grooveSize.x + groovePos.x);
			newPos.y = glm::clamp(newPos.y, 0.0f + groovePos.y, grooveSize.y + groovePos.y);

			// Notify about slider change
			signal_dragged.notify((newPos.x-groovePos.x)/grooveSize.x);
			signal_draggedXY.notify( glm::vec2( (newPos.x - groovePos.x) / grooveSize.x,(newPos.y - groovePos.y) / grooveSize.y ) );
			tr->setPos(glm::vec3(newPos,0));
		}
    });
}


float Draggable::pos() const {
	omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());
	glm::vec3 bmin, bmax;
	tr->getBounds(bmin, bmax);

	omen::ui::View* v = dynamic_cast<omen::ui::View*>(entity());
	float minX, maxX;

	omen::Transform* parentTr = nullptr;
	glm::vec3 pbmin, pbmax;

	if (v != nullptr) {
		omen::ui::View* parentView = v->parentView();
		if (parentView != nullptr) {
			parentTr = const_cast<Transform*>(parentView->getComponent<Transform>());
			parentTr->getBounds(pbmin, pbmax);
			minX = parentTr->pos().x + pbmin.x;
			maxX = parentTr->pos().x + pbmax.x - bmax.x;
		}
	}
	return (tr->pos().x - minX ) / (minX - maxX);
}

/**
  pos between 0..1
*/
void Draggable::setPos(float pos) {
	omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());
	glm::vec3 bmin, bmax;
	tr->getBounds(bmin, bmax);

	omen::ui::View* v = dynamic_cast<omen::ui::View*>(entity());
	float minX, maxX;

	omen::Transform* parentTr = nullptr;
	glm::vec3 pbmin, pbmax;

	if (v != nullptr) {
		omen::ui::View* parentView = v->parentView();
		if (parentView != nullptr) {
			parentTr = const_cast<Transform*>(parentView->getComponent<Transform>());
			parentTr->getBounds(pbmin, pbmax);
			minX = parentTr->pos().x + pbmin.x;
			maxX = parentTr->pos().x + pbmax.x - bmax.x;
		}
	}
	glm::vec2 newPos;
	newPos.x = minX + (maxX - minX)*pos;

	newPos.x = max(newPos.x, minX);
	newPos.x = min(newPos.x, maxX);
	tr->pos().x = newPos.x;
	tr->pos().y = parentTr->pos().y + (pbmax - pbmin).y / 2 - bmax.y / 2;
}

Draggable::~Draggable() {

}

void Draggable::onAttach(Entity *e) {
    m_entity = e;

	omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());
	omen::Transform* ptr = nullptr;
	if (entity()->parent() != nullptr)
		ptr = const_cast<Transform*>(entity()->parent()->getComponent<Transform>());

	if (tr != nullptr)
	{
		glm::vec3 bmin, bmax;
		tr->getBounds(bmin, bmax);
		glm::vec3 bounds = bmax - bmin;


		if (ptr != nullptr && this->m_grooveSize.x == -1.0f && this->m_grooveSize.y == -1.0f)
		{
			glm::vec3 pbmin, pbmax;
			ptr->getBounds(pbmin, pbmax);
			m_grooveSize = glm::vec3(pbmax.x, pbmax.y, 0.0f);
		}
		m_grooveSize -= glm::vec3(bounds.x, 0.0, 0.0);
	}
}

void Draggable::onDetach(Entity *e) {
    m_entity = nullptr;
}
