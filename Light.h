//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#ifndef OMEN_LIGHT_H
#define OMEN_LIGHT_H

#include <glm/glm.hpp>
#include "component/Transform.h"

namespace Omen {
    class Light {
    protected:
        Light(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color, float intensity) :
                m_transform(position, rotation, scale), m_color(color), m_intensity(intensity) { };
    public:
        typedef enum {
            POINT,
            AREA,
            DIRECT
        } LightType;

        Transform &tr() { return m_transform; }

        glm::vec3 color() { return m_color; }

        float &intensity() { return m_intensity; }

        virtual LightType type() const = 0;

    private:
        float m_intensity;
        glm::vec3 m_color;
        Transform m_transform;
    };
} // namespace Omen

#endif //OMEN_LIGHT_H
