//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#include "Clickable.h"
#include "Transform.h"
#include "../Entity.h"
#include "../Engine.h"
#include "MouseInput.h"
#include "SpriteRenderer.h"
#include "Sprite.h"

using namespace omen::ecs;

Clickable::Clickable() :
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
                signal_entity_clicked.notify(entity(),m_cursorPos);
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
			//glm::vec2 newPos = { x - m_deltaPos.x, y - m_deltaPos.y };
			glm::vec2 newPos = { x - m_deltaPos.x, tr->pos().y };
			newPos.x = max(newPos.x, 100);
			newPos.x = min(newPos.x, 390);
			tr->pos().x = newPos.x;
			tr->pos().y = newPos.y;
			
			// Notify about slider change
			signal_slider_dragged.notify((newPos.x - 100.0f) / (390.0f - 100.0f));

			SpriteRenderer* sr = const_cast<SpriteRenderer*>(entity()->getComponent<SpriteRenderer>());
			if (sr != nullptr) {
				Sprite* sprite = sr->sprite();
				sprite->setPos(newPos);
			}
		}
    });
}

Clickable::~Clickable() {

}

void Clickable::onAttach(Entity *e) {
    m_entity = e;
}

void Clickable::onDetach(Entity *e) {
    m_entity = nullptr;
}
