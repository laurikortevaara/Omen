//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_INPUTSYSTEM_H
#define OMEN_INPUTSYSTEM_H


#include "System.h"

namespace omen {
    namespace ecs {
        class InputSystem : public ecs::System {
			virtual void shutDown() {};
        };
    } // namespace omen;
} // namespace omen


#endif //OMEN_INPUTSYSTEM_H
