//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#include "Clickable.h"
#include "Transform.h"
#include "../Entity.h"
#include "../Engine.h"
#include "MouseInput.h"

using namespace omen::ecs;

Clickable::Clickable(Entity* e) {
    e->addComponent(shared_from_this());
    Engine::instance()->findComponent<MouseInput>()->
            signal_mousebutton_pressed.connect([&](int button, int action, int mods) -> void {
        if (action != GLFW_PRESS)
            return;
        std::shared_ptr<Transform> tr = entity()->getComponent<Transform>();
        if(tr!=nullptr){
            glm::vec3 bmin, bmax;
            tr->getBounds(bmin,bmax);
            if(m_cursorPos.x >= (tr->pos().x+bmin.x) &&
               m_cursorPos.x <= (tr->pos().x+bmax.x) &&
               m_cursorPos.y >= (tr->pos().y+bmin.y) &&
               m_cursorPos.y <= (tr->pos().y+bmax.y)) {
                signal_entity_clicked.notify(entity(),m_cursorPos);
            }
        }
    });

    Engine::instance()->findComponent<MouseInput>()->
            signal_cursorpos_changed.connect([&](float x, float y) -> void {
        m_cursorPos = glm::vec2(x,y);

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
