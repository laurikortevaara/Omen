//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "Entity.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "component/MouseInput.h"
#include "component/Picker.h"
#include "component/Draggable.h"
#include "Engine.h"

using namespace omen::ecs;

bool Entity::dragging = false;

Entity::Entity(const std::string &name) :
	Object(name), m_parent(nullptr), m_layer(-1), m_is_hovered(false), m_is_pressed(false), m_is_selected(false)
{
	// NO gravity for generic entity
	setGravity(0);

	std::unique_ptr<Transform> tr = std::make_unique<Transform>();
	tr->pos().x = 0;
	tr->pos().y = 0;
	tr->setBounds(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	addComponent(std::move(tr));

	auto fn = [this](omen::ecs::Entity* obj, glm::vec3 intersectPos) {
		//this->m_is_selected = obj == this;			
	};
	Picker::signal_object_picked.connect(this, fn);

	Draggable::signal_drag_started.connect(this, [this](omen::ecs::Draggable* e, float value)
	{
		dragging = true;
	});

	Draggable::signal_drag_stopped.connect(this, [this](omen::ecs::Draggable* e, float value)
	{
		dragging = false;
	});

	/**
	Make Entity hoverable by mouse
	**/
	Engine::instance()->findComponent<MouseInput>()->
		signal_cursorpos_changed.connect(this, [this](float x, float y) -> void {
		// Very slow function, just return!!!
		// TODO: Lauri fix this
		
		if (dragging)
			return;

		if (m_is_selected && this->name().compare("DIRECTION_RIGHT") == 0)
		{
			if (Engine::instance()->findComponent<MouseInput>()->mouseButtonStatesLR()[0])
			{
				float dx = x - m_cursorPos.x;
				float dy = y - m_cursorPos.y;
				parent()->tr()->pos().x += dx;
				parent()->tr()->pos().z += dy;
			}
		}
		m_cursorPos = glm::vec2(x, y);
		
		if (Entity::tr() != nullptr) {
			glm::vec3 bmin, bmax;
			Entity::tr()->getBounds(bmin, bmax);
			const glm::vec3 p = this->pos();
			//std::cout << "Testing (" << m_cursorPos.x << ", " << m_cursorPos.y << "): " << this->name() << ", pos_x: " << p.x << ", " << p.y << ", bmax: " << bmax.x << ", bmay: " << bmax.y << "\n";
			if (m_cursorPos.x >= (p.x + bmin.x) &&
				m_cursorPos.x <= (p.x + bmax.x) &&
				m_cursorPos.y >= (p.y + bmin.y) &&
				m_cursorPos.y <= (p.y + bmax.y)) {
				m_deltaPos = glm::vec2(m_cursorPos.x - pos().x, m_cursorPos.y - pos().y);
				signal_hovered.notify(this, m_cursorPos);
				if (!m_is_hovered)
					signal_entered.notify(this, m_cursorPos);
				//std::cout << "Found: " << this->name() << ", pos: " << p.x << ", " << p.y << ", bmax: " << bmax.x << "\n";
				m_is_hovered = true;
			}
			else {
				if (m_is_hovered)
					signal_exited.notify(this, m_cursorPos);
				m_is_hovered = false;
			}
		}
	});

	Engine::instance()->findComponent<MouseInput>()->signal_mousebutton_pressed.connect(this, [&](int i1, int i2, int i3, const glm::vec2& cursorPos)
	{
		if (this->hovered())
			this->setPressed(true);
	});

	Engine::instance()->findComponent<MouseInput>()->signal_mousebutton_released.connect(this, [&](int i1, int i2, int i3, const glm::vec2& cursorPos)
	{
		this->setPressed(false);
	});
}

Entity::~Entity()
{
	signal_entity_destructed.notify(this);
}

bool Entity::addChild(std::unique_ptr<Entity> e) {
	if(std::find(m_children.begin(), m_children.end(), e)==m_children.end())
	{
		e->m_parent = this;
		Engine::instance()->scene()->signal_entity_added.notify(e.get());
		m_children.push_back(std::move(e));		
		return true;
	}
	else 
		return false;
}

void Entity::onSizeChanged(glm::vec3 size, glm::vec3 oldSize)
{
	//std::cout << "OnSizeChanged: " << name() << ", size: " << size.x << ", " << size.y << "\n";
	signal_size_changed.notify(this, size, oldSize);
}

bool Entity::removeChild(std::unique_ptr<Entity> e) { 
	std::vector<std::unique_ptr<Entity>>::iterator iter = 
		std::find(m_children.begin(), m_children.end(), e); 
	if(iter!=m_children.end())
	{
		m_children.erase(iter);
		e->m_parent=nullptr;
		return true;
	}
	else 
		return false;
}

bool Entity::addComponent(std::unique_ptr<Component> c) {
    if(std::find(m_components.begin(), m_components.end(), c)==m_components.end()){
		c->setEntity(this);
		m_components.push_back(std::move(c));
        return true;
    }
    else
        return false;
}
bool Entity::removeComponent(std::unique_ptr<Component> c) {
    std::vector<std::unique_ptr<Component>>::iterator iter = std::find(m_components.begin(), m_components.end(), c);
    if(iter!=m_components.end()){m_components.erase(iter);c->setEntity(nullptr);
        return true;
    }
    else
        return false;
}

/*
template<class type>
const type* Entity::getComponent(const std::string &component_name) {
	for (auto& c : m_components)
		if (std::dynamic_pointer_cast<type*>(c.get()) != nullptr)
			return std::dynamic_pointer_cast<type*>(c.get());
	return nullptr;
}*/

Entity* Entity::findChild(const std::string& name) {
	for (const auto& child : m_children) {
		if (child->name() == name)
			return child.get();
		Entity* matchingChild = child->findChild(name);
		if (matchingChild != nullptr)
			return matchingChild;
	}
	return nullptr;
}

Entity const* Entity::findChild_const(const std::string& name) const {
	for (const auto& child : m_children) {
		if (child->name() == name)
			return child.get();
		Entity const* matchingChild = child->findChild_const(name);
		if (matchingChild != nullptr)
			return matchingChild;
	}
	return nullptr;
}


/**
Returns the global position
*/
glm::vec3 Entity::pos() const
{
	glm::vec3 gPos = localPos();
	if (parent() != nullptr)
	{
		Entity* p = dynamic_cast<Entity*>(parent());
		if (p != nullptr)
			gPos += p->pos();
	}
	if (parent() != nullptr)
	{
		if (gravity() & VERTICAL_CENTER)
			gPos.y = parent()->pos().y + (parent()->size().y-size().y)*0.5f;
		if (gravity() & HORIZONTAL_CENTER)
			gPos.x = parent()->pos().x + (parent()->size().x - size().x)*0.5f;
	}
	return gPos;
}

/**
Returns the global position
*/
glm::vec2 Entity::pos2D() const
{
	glm::vec2 gPos = localPos2D();
	if (parent() != nullptr)
	{
		Entity* p = dynamic_cast<Entity*>(parent());
		if (p != nullptr)
			gPos += p->pos2D();
	}
	return gPos;
}

void Entity::setWidth(float width) 
{ 
	glm::vec3 oldSize = tr()->boundsMax();
	glm::vec3 bmax = tr()->boundsMin(); 
	bmax.x += width; 
	bmax.y += oldSize.y; 
	bmax.z += oldSize.z; 

	tr()->setBounds(tr()->boundsMin(), bmax); 
	onSizeChanged(tr()->boundsMax() - tr()->boundsMin(), oldSize); 
}

void Entity::setHeight(float height) 
{ 
	glm::vec3 oldSize = tr()->boundsMax();
	glm::vec3 bmax = tr()->boundsMin(); 
	bmax.x += oldSize.x;
	bmax.y += height; 
	bmax.z += oldSize.z; 
	tr()->setBounds(tr()->boundsMin(), bmax); 
	onSizeChanged(tr()->boundsMax() - tr()->boundsMin(), oldSize); 
}

void Entity::setDepth(float depth) 
{ 
	glm::vec3 oldSize = size(); 
	glm::vec3 bmax = tr()->boundsMin(); 
	bmax.x += oldSize.x; 
	bmax.y += oldSize.y; 
	bmax.z += depth; 
	tr()->setBounds(tr()->boundsMin(), bmax); 
	onSizeChanged(tr()->boundsMax() - tr()->boundsMin(), oldSize); 
}