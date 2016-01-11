//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_SCENE_H
#define OMEN_SCENE_H


#include <vector>
#include "Model.h"

namespace Omen {
    class Scene {
    public:
        Scene();

        virtual ~Scene();
        
    public:
        std::vector< std::shared_ptr<Model> > m_models;

        void render(const glm::mat4 &viewProjection, const glm::mat4 &view);

        void createGround();
    };
} // namespace Omen

#endif //OMEN_SCENE_H
