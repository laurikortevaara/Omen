//
// Created by Lauri Kortevaara(Intopalo) on 10/01/16.
//

#include <iostream>
#include "Picker.h"
#include "../Engine.h"
#include "MouseInput.h"
#include "KeyboardInput.h"
#include "MeshController.h"
#include "../ui/Button.h"
#include "../ui/View.h"
#include <memory>

using namespace omen;

Picker::ObjectPicked_t Picker::signal_object_picked;
glm::vec3 Picker::IntersectPos = glm::vec3(0);
omen::ecs::Entity* Picker::CurrentlySelected = nullptr;

class Segment {
public:
    Segment(glm::vec3 s, glm::vec3 e) : start(s), end(e) { };
    glm::vec3 start, end;
};

class Ray {
public:
    Ray() { };
    glm::vec3 origin, direction;

	static bool rayIntersectsTriangle(glm::vec3& p, glm::vec3 d,
		glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, float& t) {

		glm::vec3 e1, e2, h, s, q;
		float a, f, u, v;
		e1 = v1 - v0;
		e2 = v2 - v0;

		h = glm::cross(d,e2);
		a = glm::dot(e1, h);

		if (a > -0.00001 && a < 0.00001)
			return(false);

		f = 1 / a;
		s = p - v0;
		u = f * (glm::dot(s, h));

		if (u < 0.0 || u > 1.0)
			return(false);

		q= glm::cross(s, e1);
		v = f * glm::dot(d, q);

		if (v < 0.0 || u + v > 1.0)
			return(false);

		// at this stage we can compute t to find out where
		// the intersection point is on the line
		t = f * glm::dot(e2, q);

		if (t > 0.00001) // ray intersection
			return(true);

		else // this means that there is a line intersection
			 // but not a ray intersection
			return (false);
	}

    bool raySlabIntersect(float smin, float smax, float r0, float r1, float &t0, float &t1) {
        float raydir = r1 - r0;

        if (fabs(raydir) < std::numeric_limits<float>::min()) {
            if (r0 < smin || r0 > smax) {
                return false;
            }
            else {
                return true;
            }
        }

        float ts0 = (smin - r0) / raydir;
        float ts1 = (smax - r0) / raydir;

        if (ts0 > ts1) {
            std::swap(ts0, ts1);
        }

        if (t0 > ts1 || ts0 > t1) {
            return false;
        }
        else {
            t0 = std::max(t0, ts0);
            t1 = std::min(t1, ts1);
            return true;
        }
    }

    bool segmentAABBoxIntersect(const std::unique_ptr<BoundingBox> &aabb, const Segment &s, float &intersect) {
        float v0 = 0.0f, v1 = 1.0f;

        if (!raySlabIntersect(aabb->Min().x + aabb->tr().pos().x, aabb->Max().x + aabb->tr().pos().x, s.start.x, s.end.x,
                              v0, v1))
            return false;

        // test Y slab
        if (!raySlabIntersect(aabb->Min().y + aabb->tr().pos().y, aabb->Max().y + aabb->tr().pos().y, s.start.y, s.end.y,
                              v0, v1))
            return false;

        if (!raySlabIntersect(aabb->Min().z + aabb->tr().pos().z, aabb->Max().z + aabb->tr().pos().z, s.start.z, s.end.z,
                              v0, v1))
            return false;

        intersect = v0;
        return true;
    }


};


Picker::Picker() {
    Engine::instance()->findComponent<MouseInput>()->
            signal_mousebutton_pressed.connect(this,[&](int button, int action, int mods, const glm::vec2& cursorPos) -> void {
        if (action != GLFW_PRESS)
            return;
        pick();
    });

    /*
	Engine::instance()->findComponent<MouseInput>()->
            signal_cursorpos_changed.connect(this,[&](double x, double y) -> void {
        pick();
    });
	*/

    Engine::instance()->findComponent<omen::KeyboardInput>()->signal_key_release.connect(this,[&](int key, int action, int mods, int t){
		if (key == GLFW_KEY_ESCAPE) {
			IntersectPos = glm::vec3(0);
			signal_object_picked.notify(nullptr, IntersectPos);
		}
    });
}


void Picker::pick(omen::ecs::Entity* parentEntity) {
    MouseInput *mi = Engine::instance()->findComponent<MouseInput>();
    Camera *camera = Engine::instance()->camera();

    //unproject twice to build a ray from near to far plane"
    glm::ivec4 viewport;
    glGetIntegerv(GL_VIEWPORT, (int *) &viewport);
	int dy = Engine::instance()->window()->height() - viewport[3];
    glm::vec3 v0 = glm::unProject(glm::vec3(mi->cursorPos().x, viewport[3] - mi->cursorPos().y + dy, 0.0f), camera->view(),
                                  camera->projection(), viewport);
    glm::vec3 v1 = glm::unProject(glm::vec3(mi->cursorPos().x, viewport[3] - mi->cursorPos().y + dy, 1.0f), camera->view(),
                                  camera->projection(), viewport);

    Ray ray;
    ray.origin = camera->position();
    ray.direction = glm::normalize(v1 - v0);

    Scene *scene = Engine::instance()->scene();
    float min_intersect =  std::numeric_limits<float>::max();
    ecs::Entity* pSelected = nullptr;
	IntersectPos = glm::vec3(min_intersect);
	const std::vector<std::unique_ptr<omen::ecs::Entity>>& entities = parentEntity==nullptr?scene->entities():parentEntity->children();
    for (const auto& entity : entities) {
		if (entity->getComponent<ecs::MeshController>()!=nullptr) {
			omen::ecs::MeshController* ctr = entity->getComponent<ecs::MeshController>();
			const std::unique_ptr<Mesh>& mesh = ctr->mesh();
			float intersect;
			const std::unique_ptr<BoundingBox>& b = mesh->boundingBox();
			b->tr() = *entity->tr();
			std::string name = entity->name();
			if (ray.segmentAABBoxIntersect(b, { v0, (v1 - v0) * 1000.0f }, intersect)) {
				if (intersect < min_intersect) {
					min_intersect = intersect;
					pSelected = entity.get();
					IntersectPos = v0 + (v1-v0) * 1000.0f * intersect;
				}
			}
		}
		else if(dynamic_cast< omen::ui::View* >(entity.get())) {
			omen::ui::View* v = dynamic_cast<omen::ui::View*>(entity.get());
			glm::vec2 pos = v->pos2D();
			glm::vec3 size = v->size();
			if (mi->cursorPos().x >= pos.x && mi->cursorPos().x <= (pos.x + size.x) &&
				mi->cursorPos().y >= pos.y && mi->cursorPos().y <= (pos.y + size.y)) {
				//pSelected = entity.get();
			}
		}
		if (!entity->children().empty())
			pick(entity.get());
    }

	/** Precise triangle intersection */
	if (false && pSelected != nullptr) {
		omen::ecs::MeshController* ctr = pSelected->getComponent<ecs::MeshController>();
		omen::Mesh* m = ctr->mesh().get();
		if (m->vertices().size() == 0)
			return;
		for (unsigned int vi = 0; vi < ctr->mesh()->vertexIndices().size() - 3; ++vi) 
		{
			
			std::vector<omen::Mesh::Vertex>& vs = m->vertices();
			glm::vec3 p1 = vs[m->vertexIndices()[vi]];
			glm::vec3 p2 = vs[m->vertexIndices()[vi+1]];
			glm::vec3 p3 = vs[m->vertexIndices()[vi+2]];

			p1 = glm::vec3((pSelected->tr()->tr() * glm::vec4(p1,1)));
			p2 = glm::vec3((pSelected->tr()->tr() * glm::vec4(p2, 1)));
			p3 = glm::vec3((pSelected->tr()->tr() * glm::vec4(p3, 1)));
			float t;
			if (Ray::rayIntersectsTriangle(v0, glm::normalize(v1 - v0), p1, p2, p3, t)) {
				IntersectPos = v0 + glm::normalize(v1 - v0)*t;
			}
		}
	}
	CurrentlySelected = pSelected==nullptr?CurrentlySelected:pSelected;
	if(pSelected!=nullptr && dynamic_cast<omen::ecs::GameObject*>(pSelected)!=nullptr)
		signal_object_picked.notify(pSelected, IntersectPos);
}

Picker::~Picker() {

}

void Picker::onAttach(ecs::Entity *e) {
    m_entity = e;
}

void Picker::onDetach(ecs::Entity *e) {
    m_entity = nullptr;
}
