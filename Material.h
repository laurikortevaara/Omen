//
// Created by Lauri Kortevaara on 27/12/15.
//

#ifndef OMEN_MATERIAL_H
#define OMEN_MATERIAL_H


#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include "Texture.h"

namespace omen {
    class Material {
        glm::vec4 m_diffuse_color;
        glm::vec4 m_ambient_color;
        glm::vec4 m_specular_color;
		glm::vec4 m_emissive_color;
        GLfloat m_spec_coeff;
        Texture *m_texture;
		Texture *m_texture_normal;
		Texture *m_texture_specular;
		Texture *m_texture_displacement;
        Texture *m_matcapTexture;
        bool m_isTwoSided;
    public:
        Material();

        glm::vec4 &diffuseColor() { return m_diffuse_color; }
		glm::vec4 const &const_diffuseColor()const { return m_diffuse_color; }

        void setDiffuseColor(glm::vec4 color) { m_diffuse_color = color; }

        glm::vec4 &ambientColor() { return m_ambient_color; }

        void setAmbientColor(glm::vec4 color) { m_ambient_color = color; }

        glm::vec4 &specularColor() { return m_specular_color; }

        void setSpecularColor(glm::vec4 color) { m_specular_color = color; }

		glm::vec4 const &const_emissiveColor() const { return m_emissive_color; }
		glm::vec4 &emissiveColor() { return m_emissive_color; }

		void setEmissiveColor(glm::vec4 color) { m_emissive_color = color; }

        float &specularCoeff() { return m_spec_coeff; }

        void setSpecularCoeff(float coeff) { m_spec_coeff = coeff; }

        Texture *texture() const { return m_texture; }

        void setTexture(Texture *texture) { m_texture = texture; }

		Texture *texture_normal() const { return m_texture_normal; }

		void setTextureNormal(Texture *texture_normal) { m_texture_normal = texture_normal; }

        Texture *matcapTexture() { return m_matcapTexture; }

        void setMatcapTexture(Texture *texture) { m_matcapTexture = texture; }

        bool twoSided() const { return m_isTwoSided; }

        void setTwoSided(bool twoSided = true) { m_isTwoSided = twoSided; }
    };
} // namespace omen


#endif //OMEN_MATERIAL_H
