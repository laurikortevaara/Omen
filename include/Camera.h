//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_CAMERA_H
#define OMEN_CAMERA_H

#include <glm/glm.hpp>
#include "GameObject.h"
#include "component/JoystickInput.h"

namespace omen {
    class Camera : public ecs::GameObject {

        bool m_bIsValid;
        float m_fov;
        float m_near, m_far;
        float m_yaw, m_pitch, m_roll;

        glm::vec3 m_pos;
        glm::vec3 m_lookAt;

        glm::vec3 m_up;
        glm::vec3 m_forward;

        glm::mat4x4 m_viewProj;

        glm::vec3 m_acceleration;
        glm::vec3 m_velocity;

        glm::mat4 m_view;
		glm::mat4 m_rotation;
        glm::mat4 m_projection;

        Joystick *m_joystick;

		// Camera parameters
		const float SENSOR_ASPECT_DSLR = 3.0f/2.0f; // 3/2 aspect
		
		const glm::vec2 SEONSOR_SIZE_FULL_FRAME_35MM = { 36.0, 36.0 / SENSOR_ASPECT_DSLR };
		const glm::vec2 SEONSOR_SIZE_CANON_APS_H = { 28.7, 19.0 }; // Canon
		const glm::vec2 SEONSOR_SIZE_CANON_APS_C = { 22.2, 14.8 }; // Canon
		const glm::vec2 SEONSOR_SIZE_NIKON_APS_C = { 25.1, 25.1 / SENSOR_ASPECT_DSLR }; // Nikon, Sony, Pentax, Fuji, etc..
		const glm::vec2 SEONSOR_SIZE_SIGMA_FOVEON = { 20.7, 13.8 }; // Sigma
		const glm::vec2 SENSOR_SIZE_FOUR_THIRDS = { 17.3,13.0 }; // Olympus, Panasonic, etc.
		
		const glm::vec2 DEFAULT_SENSOR_SIZE = SEONSOR_SIZE_FULL_FRAME_35MM;

		const float DEFAULT_FOCA_LENGTH = 50.0f; // 50mm

		glm::vec2 m_sensorSize; // 
		float	  m_focalLength;


    public:
        Camera(const std::string &name, const glm::vec3 &pos, const glm::vec3 &lookAt, float fov);

		void onWindowSizeChanged(int width, int height);

        void updateViewProjection();


        glm::mat4x4 &viewProjection();
        glm::mat4x4 &projection();
        glm::mat4x4 &view();
		glm::mat4x4 &rotation(); // Rotation matrix;
		glm::mat4x4 inverseRotation();

        void setPosition(glm::vec3 position);
        glm::vec3 position() const {return m_pos;}

		float focalLength() const { return m_focalLength; }
		float fov() const; // The horizontal field of view in degrees
		float horizontalFieldOfView() const; // As above
		float verticalFieldOfView() const; // The vertical field of view;

		void setFov(float fov); // Set the horizontal field of view in degrees
		void setFocalLength(float focalLength); // Set the focal length

		float zNear() const { return m_near; }
		float zFar() const { return m_far; }
        float & yaw();

		void setNear(float znear) { m_near = znear; }
		void setFar(float zfar) { m_far = zfar; }

        float & pitch();

        glm::vec3 & acceleration();
        glm::vec3 & velocity();


    };
}

#endif //OMEN_CAMERA_H
