//
// Created by Lauri Kortevaara on 10/01/16.
//

#ifndef OMEN_CAMERACONTROLLER_H
#define OMEN_CAMERACONTROLLER_H


#include "Component.h"
#include "../Camera.h"

namespace omen {
    class CameraController : public ecs::Component {
    protected:
        virtual void onAttach(ecs::Entity* e);
        virtual void onDetach(ecs::Entity* e);
    public:
        CameraController();
        virtual ~CameraController();

        void setCamera(Camera* camera) {m_camera=camera;}
    private:
        Camera* m_camera;
        Joystick *m_joystick;
    };

} // namespace omen;


#endif //OMEN_CAMERACONTROLLER_H
