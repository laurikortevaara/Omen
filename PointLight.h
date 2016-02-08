//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#ifndef OMEN_POINTLIGHT_H
#define OMEN_POINTLIGHT_H

#include <glm/glm.hpp>
#include "component/Transform.h"
#include "Light.h"

namespace omen {
    class PointLight : public Light {
    public:
        PointLight(const glm::vec3 &position,
                   const glm::vec3 &color,
                   float intensity) : Light(position,{},{}, color, intensity) { }

        virtual LightType type() const {return POINT;}
    };
} // namespace omen


#endif //OMEN_POINTLIGHT_H
