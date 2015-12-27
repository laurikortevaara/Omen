//
// Created by Lauri Kortevaara(personal) on 27/12/15.
//

#ifndef OMEN_MATERIAL_H
#define OMEN_MATERIAL_H


#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include "Texture.h"

namespace Omen {
    class Material {
        glm::vec4 m_color;
        Texture *m_texture;
    public:
        Material();

        glm::vec4 &color() { return m_color; }
        void setColor(glm::vec4 color) { m_color = color; }

        Texture* texture() { return m_texture; }
        void setTexture(Texture* texture){m_texture = texture;}
    };
} // namespace Omen


#endif //OMEN_MATERIAL_H
