//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <OpenGL/gl.h>
#include "Model.h"
#include <memory>

Model::Model() {
    m_mesh = std::make_shared<Mesh>(Mesh());
}

void Model::render() {
    m_mesh->render();
}

void Model::onCameraChanged(std::shared_ptr<Camera> camera) {
    m_mesh->m_viewMatrix = camera->view;
    m_mesh->m_projectionMatrix = camera->projection;
}
