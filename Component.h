//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_COMPONENT_H
#define OMEN_COMPONENT_H

namespace Omen {
    class Component {

    public:
        Component(){};
        virtual ~Component(){};
        void update(double time, double deltaTime);
    };
}


#endif //OMEN_COMPONENT_H
