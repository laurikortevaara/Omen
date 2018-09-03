//
// Created by Lauri Kortevaara on 11/01/16.
//

#ifndef OMEN_LIGHT_H
#define OMEN_LIGHT_H

#include <glm/glm.hpp>
#include "component/Transform.h"

namespace omen {
    class Light {
    protected:
        Light(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color, float intensity) :
                m_transform(position, rotation, scale), m_color(color), m_intensity(intensity), m_falloff(InverseSquare), m_bCastShadows(true) { };
    public:
		typedef enum {
			Constant,
			InverseLinear,
			InverseSquare,
			InverseCubic
		} LightFalloff;

        typedef enum {
            POINT,
            AREA,
            DIRECT,
			SPOT,
			VOLUME // Volumetric light not currently implemented
        } LightType;

        Transform &tr() { return m_transform; }

        glm::vec3 color() { return m_color; }

        float &intensity() { return m_intensity; }
		
		bool castShadows() const { return m_bCastShadows; }
		void setCastShadows(bool cast) { m_bCastShadows = cast; }

        virtual LightType type() const = 0;
		
		const LightFalloff DEFAULT_FALLOFF_TYPE = InverseSquare;
		LightFalloff falloff() const { return m_falloff; }
		void setFalloff(LightFalloff falloff) { m_falloff = falloff; }

		const float DEFAULT_FALLOFF_DISTANCE = 25.0f;
		float falloffDistance() const { return m_falloffDistance; }
		void setFalloffDistance(float distance) { m_falloffDistance = distance; }

		const std::string& name() const { return m_name; }
		void setName(const std::string& name) { m_name = name; }

		void setPosition(glm::vec3& position) { m_transform.setPos(position); }
		const glm::vec3& position() { return m_transform.pos(); }

    private:
        float m_intensity;
        glm::vec3 m_color;
        Transform m_transform;
		LightFalloff m_falloff;
		float	m_falloffDistance;
		bool m_bCastShadows;
		std::string m_name;

    };

	class PointLight : public Light {
	public:
		PointLight(const glm::vec3 &position,
			const glm::vec3 &color,
			float intensity) : Light(position, {}, {}, color, intensity) { }

		virtual LightType type() const { return POINT; }
	};

	class DirectionalLight : public Light {
	public:
		DirectionalLight(const glm::vec3 &direction,
			const glm::vec3 &color,
			float intensity) : Light(direction, {}, {}, color, intensity) { }

		virtual LightType type() const { return DIRECT; }
	};

	class AreaLight : public Light {
	public:
		typedef enum {
			Rectangle,
			Sphere
		} AreaLightShape;

		AreaLight(const glm::vec3 &direction,
			const glm::vec3 &color,
			float intensity,
			AreaLightShape shape = Rectangle) : Light(direction, {}, {}, color, intensity), m_shape(shape) { }

		virtual LightType type() const { return AREA; }
		AreaLightShape shape() const { return m_shape; }
	private:
		AreaLightShape m_shape;
	};

	class SpotLight : public Light {
	public:
		SpotLight(
			const glm::vec3 &position,
			const glm::vec3 &rotation,
			const glm::vec3 &color,
			float intensity,
			float size, /** Cone Size in Degrees, e.g. the outer angle **/
			float innerAngle = 0.0f) : Light(position, rotation, {}, color, intensity), m_size(size), m_innerAngle(innerAngle) { }

		const float size() const { return m_size; }

		virtual LightType type() const { return SPOT; }
	private:
		float m_size;
		float m_innerAngle;
	};
} // namespace omen

#endif //OMEN_LIGHT_H
