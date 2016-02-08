//
// Created by Lauri Kortevaara(Intopalo) on 10/01/16.
//

#ifndef OMEN_TRANSFORMER_H
#define OMEN_TRANSFORMER_H


#include "Component.h"
#include "../Camera.h"
#include "../Mesh.h"

namespace omen {
    class Transformer : public ecs::Component {
    protected:
        virtual void onAttach(ecs::Entity* e);
        virtual void onDetach(ecs::Entity* e);
    public:
        Transformer();
        virtual ~Transformer();

        void setTr(Transform* tr) {m_tr=tr;}
    private:
        Transform* m_tr;
        Joystick *m_joystick;
        Mesh *m_obj;
    };

} // namespace omen;


#endif //OMEN_TRANSFORMER_H
