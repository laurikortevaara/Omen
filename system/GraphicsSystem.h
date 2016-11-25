//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_GRAPHICSSYSTEM_H
#define OMEN_GRAPHICSSYSTEM_H


#include "System.h"

namespace omen {
    namespace ecs {
        class GraphicsSystem : public ecs::System {

        public:
			virtual void add(Component *component);
            void render();

        };
    }
}

#endif //OMEN_GRAPHICSSYSTEM_H
