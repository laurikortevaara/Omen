//
// Created by Lauri Kortevaara on 27/12/15.
//

#ifndef OMEN_MATERIAL_H
#define OMEN_MATERIAL_H

#include "typedef.h"
#include <glm/glm.hpp>

namespace omen {
    class Texture;

    class Material {
        glm::vec4 m_diffuse_color;
        glm::vec4 m_ambient_color;
        glm::vec4 m_specular_color;
		glm::vec4 m_emissive_color;
		float m_diffuseIntensity; // Diffuse Intensity in Blender
        float m_specularIntensity; // Specular Intensity in Blender
		float m_shininess; // Specular Hardness in Blender
		float m_emissive_factor;
        Texture *m_texture;
		Texture *m_texture_normal;
		Texture *m_texture_shininess_exponent;
		Texture *m_texture_specular_color;
		Texture *m_texture_specular_factor;
		Texture *m_texture_displacement;
        Texture *m_matcapTexture;
        bool m_isTwoSided;
    public:
        Material();
		~Material();
		// Ambient color
		void setAmbientColor(glm::vec4 color) { m_ambient_color = color; }
		glm::vec4 &ambientColor() { return m_ambient_color; }
		glm::vec4 ambientColor() const { return m_ambient_color; }
		
		// Diffuse/Albedo color
		void setDiffuseColor(glm::vec4 color) { m_diffuse_color = color; }
        glm::vec4 &diffuseColor() { return m_diffuse_color; }
		glm::vec4 diffuseColor() const { return m_diffuse_color; }

		// Diffuuse Intensity
		void setDiffuseIntensity(float intensity) { m_diffuseIntensity = intensity; }
		float diffuseIntensity() const { return m_diffuseIntensity; }

		// Emissive color
		void setEmissiveColor(glm::vec4 color) { m_emissive_color = color; }
		glm::vec4 const &const_emissiveColor() const { return m_emissive_color; }
		glm::vec4 emissiveColor() const { return m_emissive_color; }

		// Emissive factor
		void setEmissiveFactor(float factor) { m_emissive_factor = factor; }
		float emissiveFactor() const { return m_emissive_factor; }

		// Specular color
		void setSpecularColor(glm::vec4 color) { m_specular_color = color; }
		glm::vec4 &specularColor() { return m_specular_color; }
		glm::vec4 specularColor() const { return m_specular_color; }
		
		// Specular reflection coeff/intensity
		void setSpecularIntensity(float intensity) { m_specularIntensity = intensity; }
		float specularIntensity() const { return m_specularIntensity; }

		// Specular reflection hardness / shininess
		void setShininess(float shininess) { m_shininess = shininess; }
		float shininess() const { return m_shininess; }
		
		// Diffuse texture map
		void setTexture(Texture *texture) { m_texture = texture; }
		Texture *texture() const { return m_texture; }

		// Normal texture map
		void setTextureNormal(Texture *texture_normal) { m_texture_normal = texture_normal; }
		Texture *textureNormal() const { return m_texture_normal; }

		// Specular color texture map
		void setTextureSpecularColor(Texture *texture_spec_color ) { m_texture_specular_color = texture_spec_color; }
		Texture *textureSpecularColor() const { return m_texture_specular_color; }

		// Specular factor texture map
		void setTextureSpecularFactor(Texture *texture_spec_factor) { m_texture_specular_factor = texture_spec_factor; }
		Texture *textureSpecularFactor() const { return m_texture_specular_factor; }

		// Specular exponent texture map
		void setTextureShininessExponent(Texture *texture_spec_exp) { m_texture_shininess_exponent = texture_spec_exp; }
		Texture *textureShininessExponent() const { return m_texture_shininess_exponent; }

		// Mapcap texture
		void setMatcapTexture(Texture *texture) { m_matcapTexture = texture; }
		Texture *matcapTexture() { return m_matcapTexture; }

        // Boolean flag describing whether this material is two_sided (no face culling)
		void setTwoSided(bool twoSided = true) { m_isTwoSided = twoSided; }
		bool twoSided() const { return m_isTwoSided; }
    };
} // namespace omen


#endif //OMEN_MATERIAL_H
