//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_SCENE_H
#define OMEN_SCENE_H


#include <vector>
#include "Model.h"
#include "Light.h"
#include "Sky.h"

namespace Omen {
    class Scene {
    public:
        Scene();

        virtual ~Scene();

    public:

        void render(const glm::mat4 &viewProjection, const glm::mat4 &view);

        void createGround();

        std::vector<std::shared_ptr<Model> >& models() {return m_models;}
        std::vector<std::shared_ptr<Light> >& lights() {return m_lights;}

    private:
        std::vector<std::shared_ptr<Model> > m_models;
        std::vector<std::shared_ptr<Light> > m_lights;
        Sky*    m_sky;


    };
} // namespace Omen

#endif //OMEN_SCENE_H
