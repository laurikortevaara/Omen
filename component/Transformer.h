//
// Created by Lauri Kortevaara(Intopalo) on 10/01/16.
//

#ifndef OMEN_TRANSFORMER_H
#define OMEN_TRANSFORMER_H


#include "Component.h"
#include "../Camera.h"
#include "../Mesh.h"

namespace Omen {
    class Transformer : public ecs::Component {
    public:
        Transformer();
        virtual ~Transformer();

        void setTr(Transform* tr) {m_tr=tr;}
    private:
        Transform* m_tr;
        Joystick *m_joystick;
        Mesh *m_obj;
    };

} // namespace Omen;


#endif //OMEN_TRANSFORMER_H
