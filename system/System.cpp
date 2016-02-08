//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "System.h"
#include "../Engine.h"

using namespace omen::ecs;

void System::update(double time, double deltaTime) {
    for(auto component : m_components)
        component->update(time, deltaTime);
}

System::System():
        m_isEnabled(false) {
    omen::Engine *e = omen::Engine::instance();
    e->signal_engine_update.connect([this, e](double time, double deltaTime) {
        if(m_isEnabled)
            update(time, deltaTime);
    });
}

