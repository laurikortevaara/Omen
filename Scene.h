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

public:
    std::vector< std::shared_ptr<Model> > m_models;
    std::shared_ptr<Camera> m_camera;
};


#endif //OMEN_SCENE_H
