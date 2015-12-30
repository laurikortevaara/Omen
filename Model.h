//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_MODEL_H
#define OMEN_MODEL_H


#include "Mesh.h"

namespace Omen {
    class Model {
    public:
        // ctor with pre-created mesh
        Model(std::unique_ptr<Mesh> mesh= nullptr);

        void render(const glm::mat4 &viewProjection);

        std::unique_ptr<Omen::Mesh> m_mesh;
    };
} // namespace Omen

#endif //OMEN_MODEL_H
