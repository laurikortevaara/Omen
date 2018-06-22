//
// Created by Lauri Kortevaara on 04/02/16.
//

#include "component/Clickable.h"

#include "Entity.h"
#include "Engine.h"
#include "component/MouseInput.h"
#include "component/SpriteRenderer.h"
#include "component/Sprite.h"
#include "component/Transform.h"

using namespace omen::ecs;

Clickable::Clickable() :
	m_is_pressed(false)
{
    Engine::instance()->findComponent<MouseInput>()->
            signal_mousebutton_pressed.connect(this,[&](int button, int action, int mods, const glm::vec2& cursorPos) -> void {
        if (action != GLFW_PRESS || entity() == nullptr)
            return;

		omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());

        if(tr!=nullptr){
			if(cursorInsideEntity()) {
				glm::vec3 pos = entity()->pos();
				m_deltaPos = glm::vec2(cursorPos.x-pos.x, cursorPos.y-pos.y);
                signal_entity_mouse_pressed.notify(entity(), cursorPos, button);
				m_is_pressed = true;
            }
        }
    });

	Engine::instance()->findComponent<MouseInput>()->
		signal_mousebutton_released.connect(this,[&](int button, int action, int mods, const glm::vec2& cursorPos) -> void {
		if (m_is_pressed)
			signal_entity_mouse_clicked.notify(entity(), cursorPos, button);
		m_is_pressed = false;
		if(cursorInsideEntity())
			signal_entity_mouse_released.notify(entity(), cursorPos, button);
	});

	Engine::instance()->findComponent<MouseInput>()->
		signal_cursorpos_changed.connect(this,[&](float x, float y) -> void {
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


bool Clickable::cursorInsideEntity() {
	omen::Transform* tr = const_cast<Transform*>(entity()->getComponent<Transform>());
	if (tr != nullptr)
	{
		glm::vec3 pos = entity()->pos();
		glm::vec3 bmin, bmax;
		tr->getBounds(bmin, bmax);
		return (m_cursorPos.x >= (pos.x + bmin.x) &&
			m_cursorPos.x <= (pos.x + bmax.x) &&
			m_cursorPos.y >= (pos.y + bmin.y) &&
			m_cursorPos.y <= (pos.y + bmax.y));
	}
	else
		return false;
}