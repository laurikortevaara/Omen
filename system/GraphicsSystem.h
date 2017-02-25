//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_GRAPHICSSYSTEM_H
#define OMEN_GRAPHICSSYSTEM_H


#include "System.h"

namespace omen {
	class Shader;
	namespace ecs {
        class GraphicsSystem : public ecs::System {

        public:
			virtual void add(Component *component);
            virtual void render(omen::Shader* pShader = nullptr, int layer=0);

			virtual void shutDown() {};

			int depthMap;
        };
    }
}

#endif //OMEN_GRAPHICSSYSTEM_H
