//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_SCENE_H
#define OMEN_SCENE_H


#include <vector>
#include "Model.h"

class Scene {
public:
    Scene();
    virtual ~Scene();
    void render();
private:
    std::vector< std::shared_ptr<Model> > m_models;
};


#endif //OMEN_SCENE_H
