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

glm::mat4 Transform::world_tr() 
{
	glm::mat4 parentTr;
	if (entity()->parent() != nullptr)
		parentTr = entity()->parent()->tr()->world_tr();
	glm::mat4 translation = glm::translate(glm::mat4(1), m_position);
	//glm::mat4 rotation = glm::rotate( glm::mat4(), m_rotation.x, glm::vec3(1,0,0));
	glm::mat4 rotation = glm::rotate(glm::mat4(), m_rotation.y, glm::vec3(0, 1, 0));
	//rotation = glm::rotate( rotation, m_rotation.y, glm::vec3(0,0,1));
	glm::mat4 scale = glm::scale(glm::mat4(1), m_scale);

	m_tr = translation * rotation * scale;
	return parentTr*m_tr;
}


/*
	Returns the bounding rectangle clipped by parent bounding rectangle
*/
void Transform::getClippedBounds(glm::vec3& min, glm::vec3& max)const
{
	omen::ecs::Entity* e = entity();
	glm::vec3 bmin, bmax;
	getBounds(bmin, bmax);
	if(e->parent()!=nullptr)
	{
		glm::vec3 bminp, bmaxp;
		e->parent()->tr_const()->getClippedBounds(bminp, bmaxp);
		bmin = glm::max(bmin, bminp);
		bmax = glm::min(bmax, bmaxp);
	}
	min = bmin;
	max = bmax;
}