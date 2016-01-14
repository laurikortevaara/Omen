//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#ifndef OMEN_OCEAN_H
#define OMEN_OCEAN_H

#include "Renderable.h"

namespace Omen {
    class Ocean : public Renderable {
    public:
        Ocean();
        virtual ~Ocean();
        virtual void render();

        void evaluateWaves(float t);

        int* m_innerTesselationLevel;
        int* m_outerTesselationLevel;
        std::vector<int> m_innerTessellationLevels;
        std::vector<int> m_outerTessellationLevels;
    };

} // namespace Omen


#endif //OMEN_OCEAN_H
