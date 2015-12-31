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
        glm::vec4 m_diffuse_color;
        glm::vec4 m_ambient_color;
        glm::vec4 m_specular_color;
        GLfloat m_spec_coeff;
        Texture *m_texture;
        bool m_isTwoSided;
    public:
        Material();

        glm::vec4 &diffuseColor() { return m_diffuse_color; }

        void setDiffuseColor(glm::vec4 color) { m_diffuse_color = color; }

        glm::vec4 &ambientColor() { return m_ambient_color; }

        void setAmbientColor(glm::vec4 color) { m_ambient_color = color; }

        glm::vec4 &specularColor() { return m_specular_color; }

        void setSpecularColor(glm::vec4 color) { m_specular_color = color; }

        float &specularCoeff() { return m_spec_coeff; }

        void setSpecularCoeff(float coeff) { m_spec_coeff = coeff; }

        Texture *texture() { return m_texture; }

        void setTexture(Texture *texture) { m_texture = texture; }

        bool twoSided() const { return m_isTwoSided; }

        void setTwoSided(bool twoSided = true) { m_isTwoSided = twoSided; }
    };
} // namespace Omen


#endif //OMEN_MATERIAL_H
