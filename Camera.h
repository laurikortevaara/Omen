//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_CAMERA_H
#define OMEN_CAMERA_H

#include <glm/glm.hpp>
#include "GameObject.h"
#include "component/JoystickInput.h"

namespace Omen {
    class Camera : public GameObject {

        bool m_bIsValid;
        float m_fov;
        glm::vec3 m_pos;
        glm::vec3 m_lookAt;

    public:
        Camera(const std::string &name, const glm::vec3 &pos, const glm::vec3 &lookAt, float fov);

        glm::mat4x4 &mvp();

        glm::mat4x4 m_mvp;

        void onWindowSizeChanged(int width, int height);

        glm::vec3 m_velocity;
        glm::mat4 m_view;
        float m_acceleration;
        Joystick *m_joystick;

        void updateMVP();
    };
}

#endif //OMEN_CAMERA_H
