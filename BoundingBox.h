//
// Created by Lauri Kortevaara(Intopalo) on 09/01/16.
//

#ifndef OMEN_BOUNDINGBOX_H
#define OMEN_BOUNDINGBOX_H


#include <glm/detail/type_vec3.hpp>
#include "BoundingVolume.h"
#include "component/Transform.h"

namespace Omen {
    class BoundingBox : public Omen::BoundingVolume {
    public:
        void set(glm::vec3 min, glm::vec3 max);
        glm::vec3 min() const {return m_min;}
        glm::vec3 max() const {return m_max;}
        Omen::Transform& tr() {return m_transform;}
    private:
        Omen::Transform m_transform;
        glm::vec3   m_max;
        glm::vec3   m_min;
    };
}


#endif //OMEN_BOUNDINGBOX_H