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

class Segment {
public:
    Segment(glm::vec3 s, glm::vec3 e) : start(s), end(e) { };
    glm::vec3 start, end;
};

class Ray {
public:
    Ray() { };
    glm::vec3 origin, direction;

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

    bool segmentAABBoxIntersect(BoundingBox &aabb, const Segment &s, float &intersect) {
        float v0 = 0.0f, v1 = 1.0f;

        if (!raySlabIntersect(aabb.Min().x + aabb.tr().pos().x, aabb.Max().x + aabb.tr().pos().x, s.start.x, s.end.x,
                              v0, v1))
            return false;

        // test Y slab
        if (!raySlabIntersect(aabb.Min().y + aabb.tr().pos().y, aabb.Max().y + aabb.tr().pos().y, s.start.y, s.end.y,
                              v0, v1))
            return false;

        if (!raySlabIntersect(aabb.Min().z + aabb.tr().pos().z, aabb.Max().z + aabb.tr().pos().z, s.start.z, s.end.z,
                              v0, v1))
            return false;

        intersect = v0;
        return true;
    }


};


Picker::Picker() {
    Engine::instance()->findComponent<MouseInput>()->
            signal_mousebutton_pressed.connect([&](int button, int action, int mods, const glm::vec2& cursorPos) -> void {
        if (action != GLFW_PRESS)
            return;
        pick();
    });

    Engine::instance()->findComponent<MouseInput>()->
            signal_cursorpos_changed.connect([&](double x, double y) -> void {
        pick();
    });

    Engine::instance()->findComponent<omen::KeyboardInput>()->signal_key_release.connect([&](int key, int action, int mods, int t){
        if(key==GLFW_KEY_ESCAPE)
            signal_object_picked.notify(nullptr);
    });
}

void Picker::pick() {
    MouseInput *mi = Engine::instance()->findComponent<MouseInput>();
    Camera *camera = Engine::instance()->camera();

    //unproject twice to build a ray from near to far plane"
    glm::ivec4 viewport;
    glGetIntegerv(GL_VIEWPORT, (int *) &viewport);
    glm::vec3 v0 = glm::unProject(glm::vec3(mi->cursorPos().x, viewport[3] - mi->cursorPos().y, 0.0f), camera->view(),
                                  camera->projection(), viewport);
    glm::vec3 v1 = glm::unProject(glm::vec3(mi->cursorPos().x, viewport[3] - mi->cursorPos().y, 1.0f), camera->view(),
                                  camera->projection(), viewport);

    Ray ray;
    ray.origin = camera->position();
    ray.direction = glm::normalize(v1 - v0);

    Scene *scene = Engine::instance()->scene();
    float min_intersect =  std::numeric_limits<float>::max();
    ecs::Entity* pSelected = nullptr;
    for (const auto& entity : scene->entities()) {
		if (entity->getComponent<ecs::MeshController>()!=nullptr) {
			omen::ecs::MeshController* ctr = entity->getComponent<ecs::MeshController>();
			const std::unique_ptr<Mesh>& mesh = ctr->mesh();
			float intersect;
			BoundingBox b = mesh->boundingBox();
			if (ray.segmentAABBoxIntersect(b, { v0, (v1 - v0) * 100.0f }, intersect)) {
				signal_object_picked.notify(entity.get());
				if (intersect < min_intersect) {
					min_intersect = intersect;
					pSelected = entity.get();
				}
			}
		}
		else if(dynamic_cast< omen::ui::View* >(entity.get())) {
			omen::ui::View* v = dynamic_cast<omen::ui::View*>(entity.get());
			glm::vec2 pos = v->pos2D();
			glm::vec3 size = v->size();
			if (mi->cursorPos().x >= pos.x && mi->cursorPos().x <= (pos.x + size.x) &&
				mi->cursorPos().y >= pos.y && mi->cursorPos().y <= (pos.y + size.y)) {
				pSelected = entity.get();
			}
		}
    }
	//if(pSelected!=nullptr && dynamic_cast<omen::ecs::GameObject*>(pSelected)!=nullptr)
		signal_object_picked.notify(pSelected);
}

Picker::~Picker() {

}

void Picker::onAttach(ecs::Entity *e) {
    m_entity = e;
}

void Picker::onDetach(ecs::Entity *e) {
    m_entity = nullptr;
}
