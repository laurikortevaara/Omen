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
        glm::mat4 m_projection;

        Joystick *m_joystick;


    public:
        Camera(const std::string &name, const glm::vec3 &pos, const glm::vec3 &lookAt, float fov);

        void onWindowSizeChanged(int width, int height);

        void updateViewProjection();


        glm::mat4x4 &viewProjection();
        glm::mat4x4 &projection();
        glm::mat4x4 &view();

        void setPosition(glm::vec3 position);
        glm::vec3 position() const {return m_pos;}

        float & yaw();

        float & pitch();

        glm::vec3 & acceleration();
        glm::vec3 & velocity();


    };
}

#endif //OMEN_CAMERA_H
