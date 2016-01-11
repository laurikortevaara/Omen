//
// Created by Lauri Kortevaara(Intopalo) on 10/01/16.
//

#ifndef OMEN_CAMERACONTROLLER_H
#define OMEN_CAMERACONTROLLER_H


#include "Component.h"
#include "../Camera.h"

namespace Omen {
    class CameraController : public ecs::Component {
    public:
        CameraController();
        virtual ~CameraController();

        void setCamera(Camera* camera) {m_camera=camera;}
    private:
        Camera* m_camera;
        Joystick *m_joystick;
    };

} // namespace Omen;


#endif //OMEN_CAMERACONTROLLER_H
