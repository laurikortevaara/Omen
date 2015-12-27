//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_COMPONENT_H
#define OMEN_COMPONENT_H

namespace Omen {
    namespace ecs {
        class Component {

        public:
            Component() { };

            virtual ~Component() { };

            virtual void update(double time, double deltaTime){};
        };
    } // namespace ecs
} // namespace Omen


#endif //OMEN_COMPONENT_H
