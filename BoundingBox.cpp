//
// Created by Lauri Kortevaara(Intopalo) on 09/01/16.
//

#include "BoundingBox.h"

void Omen::BoundingBox::set(glm::vec3 min, glm::vec3 max) {
    m_min = min; m_max = max;
}
