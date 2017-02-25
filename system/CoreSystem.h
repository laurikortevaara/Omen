//
// Created by Lauri Kortevaara(Intopalo) on 10/01/16.
//

#ifndef OMEN_CORESYSTEM_H
#define OMEN_CORESYSTEM_H

#include "System.h"

namespace omen {
    namespace ecs {
        class CoreSystem : public ecs::System{
        public:
			virtual void shutDown() {};
        private:
        };
    } // namepsace ecs
} // namespace omen

#endif //OMEN_CORESYSTEM_H
