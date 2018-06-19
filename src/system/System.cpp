//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "system/System.h"
#include "Engine.h"

using namespace omen::ecs;

void System::update(double time, double deltaTime) {
    for(auto component : m_components)
        component->update(time, deltaTime);
}

System::System():
		Object(typeid(this).name()),
        m_isEnabled(false) {
    omen::Engine *e = omen::Engine::instance();
    e->signal_engine_update.connect(this,[this, e](double time, double deltaTime) {
        if(m_isEnabled)
            update(time, deltaTime);
    });

	e->signal_engine_shut_down.connect(this,[this]() {
		this->shutDown();
	});
	
}

void System::add(Component* component)
{
	component->attachToSystem(this);
	component->signal_component_destructed.connect(this, [this](omen::ecs::Component* c) {
		remove(c);
	});
	m_components.push_back(component); 
	component->signal_component_destructed.connect(this,[&](Component* c) {
		if (std::find(m_components.begin(), m_components.end(), c) != m_components.end())
		{
			m_components.erase(std::find(m_components.begin(), m_components.end(), c));
		}
	});
}

void System::detachFromSystem(Component* component)
{
	std::vector<ecs::Component*>::iterator iter = std::find(m_components.begin(), m_components.end(), component);
	if(iter != m_components.end())
		m_components.erase(iter);
}