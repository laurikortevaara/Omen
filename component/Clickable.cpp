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
            signal_mousebutton_pressed.connect([&](int button, int action, int mods, const glm::vec2& cursorPos) -> void {
        if (action != GLFW_PRESS || entity() == nullptr)
            return;

		omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());

        if(tr!=nullptr){
			glm::vec3 pos = entity()->pos();
            glm::vec3 bmin, bmax;
            tr->getBounds(bmin,bmax);
            if(cursorPos.x >= (pos.x+bmin.x) &&
				cursorPos.x <= (pos.x+bmax.x) &&
				cursorPos.y >= (pos.y+bmin.y) &&
				cursorPos.y <= (pos.y+bmax.y)) {
				m_deltaPos = glm::vec2(cursorPos.x-pos.x, cursorPos.y-pos.y);
                signal_entity_clicked.notify(entity(), cursorPos);
				m_is_pressed = true;
            }
        }
    });

	Engine::instance()->findComponent<MouseInput>()->
		signal_mousebutton_released.connect([&](int button, int action, int mods, const glm::vec2& cursorPos) -> void {
		m_is_pressed = false;
	});

	Engine::instance()->findComponent<MouseInput>()->
		signal_cursorpos_changed.connect([&](float x, float y) -> void {
		m_cursorPos = glm::vec2(x, y); });
}

Clickable::~Clickable() {

}

void Clickable::onAttach(Entity *e) {
    m_entity = e;
}

void Clickable::onDetach(Entity *e) {
    m_entity = nullptr;
}
