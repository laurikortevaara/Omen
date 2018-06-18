//
// Created by Lauri Kortevaara on 09/01/16.
//

#ifndef OMEN_BOUNDINGBOX_H
#define OMEN_BOUNDINGBOX_H


#include <glm/glm.hpp>
#include "BoundingVolume.h"
#include "component/Transform.h"

namespace omen {
    class BoundingBox : public omen::BoundingVolume {
    public:
        void set(glm::vec3 min, glm::vec3 max);
        glm::vec3 Min() const {return m_min;}
        glm::vec3 Max() const {return m_max;}
        omen::Transform& tr() {return m_transform;}
		void setTr(omen::Transform& tr) { m_transform = tr; }
    private:
		// TODO: Lauri check This!!
        omen::Transform m_transform; // This does not work as it gets killed in GameObject::updateBounds
        glm::vec3   m_max;
        glm::vec3   m_min;
    };
}


#endif //OMEN_BOUNDINGBOX_H
