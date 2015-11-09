//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_MODEL_H
#define OMEN_MODEL_H


#include "Mesh.h"

class Model {
public:
    Model();
    void render();
private:
    std::shared_ptr<Mesh> m_mesh;
};


#endif //OMEN_MODEL_H
