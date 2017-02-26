//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "Transform.h"
#include "../Entity.h"

using namespace omen;

void Transform::update(double time, double deltaTime) {
}

void Transform::onAttach(omen::ecs::Entity *e) {

}

void Transform::onDetach(omen::ecs::Entity *e) {

}

glm::vec3 Transform::boundsMin() const { return m_bounds_min; }

glm::vec3 Transform::boundsMax() const 
{ 
	if (m_bounds_max.x == -1 || m_bounds_max.y == -1 || m_bounds_max.z == -1)
	{
		glm::vec3 bmin = boundsMin();

		for (const auto& c : entity()->children())
		{
			float w = c->width();
			float h = c->height();
			float d = c->depth();
			if ((m_bounds_max.x - bmin.x) < w)
				m_bounds_max.x = w - bmin.x;
			if ((m_bounds_max.y - bmin.y) < h)
				m_bounds_max.y = h - bmin.y;
			if ((m_bounds_max.z - bmin.z) < d)
				m_bounds_max.z = d - bmin.z;
		}
	}
	return m_bounds_max; 
}

