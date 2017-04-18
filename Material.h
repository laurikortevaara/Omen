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

		// Ambient color
		void setAmbientColor(glm::vec4 color) { m_ambient_color = color; }
		glm::vec4 &ambientColor() { return m_ambient_color; }
		glm::vec4 ambientColor() const { return m_ambient_color; }
		
		// Diffuse/Albedo color
		void setDiffuseColor(glm::vec4 color) { m_diffuse_color = color; }
        glm::vec4 &diffuseColor() { return m_diffuse_color; }
		glm::vec4 diffuseColor() const { return m_diffuse_color; }

		// Emissive color
		void setEmissiveColor(glm::vec4 color) { m_emissive_color = color; }
		glm::vec4 const &const_emissiveColor() const { return m_emissive_color; }
		glm::vec4 emissiveColor() const { return m_emissive_color; }

		// Specular color
		void setSpecularColor(glm::vec4 color) { m_specular_color = color; }
		glm::vec4 &specularColor() { return m_specular_color; }
		glm::vec4 specularColor() const { return m_specular_color; }
		
		// Specular reflection coeff
		void setSpecularCoeff(float coeff) { m_spec_coeff = coeff; }
		float specularCoeff() const { return m_spec_coeff; }
		
		// Diffuse texture map
		void setTexture(Texture *texture) { m_texture = texture; }
		Texture *texture() const { return m_texture; }

		// Normal texture map
		void setTextureNormal(Texture *texture_normal) { m_texture_normal = texture_normal; }
		Texture *texture_normal() const { return m_texture_normal; }

		// Mapcap texture
		void setMatcapTexture(Texture *texture) { m_matcapTexture = texture; }
		Texture *matcapTexture() { return m_matcapTexture; }

        // Boolean flag describing whether this material is two_sided (no face culling)
		void setTwoSided(bool twoSided = true) { m_isTwoSided = twoSided; }
		bool twoSided() const { return m_isTwoSided; }
    };
} // namespace omen


#endif //OMEN_MATERIAL_H
