//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_MODEL_H
#define OMEN_MODEL_H


#include "Mesh.h"
#include "Camera.h"

class Model {
public:
    Model();
    void render();

    void onCameraChanged(std::shared_ptr<Camera> camera);

    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Mesh> m_mesh;

    glm::mat4   m_modelMatrix;
};


#endif //OMEN_MODEL_H
