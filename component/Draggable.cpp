//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#include "Draggable.h"
#include "Transform.h"
#include "../Entity.h"
#include "../Engine.h"
#include "MouseInput.h"
#include "SpriteRenderer.h"
#include "Sprite.h"

using namespace omen::ecs;

Draggable::Draggable() :
	m_is_pressed(false)
{
    Engine::instance()->findComponent<MouseInput>()->
            signal_mousebutton_pressed.connect([&](int button, int action, int mods) -> void {
        if (action != GLFW_PRESS || entity() == nullptr)
            return;

	omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());

        if(tr!=nullptr){
            glm::vec3 bmin, bmax;
            tr->getBounds(bmin,bmax);
            if(m_cursorPos.x >= (tr->pos().x+bmin.x) &&
               m_cursorPos.x <= (tr->pos().x+bmax.x) &&
               m_cursorPos.y >= (tr->pos().y+bmin.y) &&
               m_cursorPos.y <= (tr->pos().y+bmax.y)) {
				m_deltaPos = glm::vec2(m_cursorPos.x-tr->pos().x, m_cursorPos.y-tr->pos().y);
                signal_clicked.notify(entity(),m_cursorPos);
				m_is_pressed = true;
            }
        }
    });

	Engine::instance()->findComponent<MouseInput>()->
		signal_mousebutton_released.connect([&](int button, int action, int mods) -> void {
		m_is_pressed = false;
	});

    Engine::instance()->findComponent<MouseInput>()->
            signal_cursorpos_changed.connect([&](float x, float y) -> void {
        m_cursorPos = glm::vec2(x,y);
		if (m_is_pressed) {
			omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());
			glm::vec3 bmin, bmax;
			tr->getBounds(bmin, bmax);
			
			glm::vec2 newPos = { x - m_deltaPos.x, tr->pos().y };
			omen::ui::View* v = dynamic_cast<omen::ui::View*>(entity());
			float minX, maxX;
			float minY, maxY;

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
			newPos.x = max(newPos.x, minX);
			newPos.x = min(newPos.x, maxX);
			tr->pos().x = newPos.x;
			tr->pos().y = parentTr->pos().y+(pbmax-pbmin).y/2-bmax.y/2;
			
			// Notify about slider change
			signal_dragged.notify((newPos.x - parentTr->pos().x) / (pbmax.x-bmax.x));

			SpriteRenderer* sr = const_cast<SpriteRenderer*>(entity()->getComponent<SpriteRenderer>());
			if (sr != nullptr) {
				Sprite* sprite = sr->sprite();
				sprite->setPos(newPos);
			}
		}
    });
}


float Draggable::pos() const {
	omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());
	glm::vec3 bmin, bmax;
	tr->getBounds(bmin, bmax);

	omen::ui::View* v = dynamic_cast<omen::ui::View*>(entity());
	float minX, maxX;
	float minY, maxY;

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
	float minY, maxY;

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
}

void Draggable::onDetach(Entity *e) {
    m_entity = nullptr;
}
