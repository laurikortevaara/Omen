//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include "Scene.h"
#include "Model.h"
#include "Camera.h"


Scene::Scene() {
    m_camera = std::make_shared<Camera>(new Camera());
    m_models.push_back(std::make_shared<Model>(Model()));
    for(const auto & model : m_models)
        m_camera->CameraChanged.connect(std::function<void(std::shared_ptr<Camera>)>([=](std::shared_ptr<Camera> camera){model->onCameraChanged(camera);}));
}

Scene::~Scene() {
    m_models.clear();
}

void Scene::render() {
    m_camera->update();
    for(const auto & model : m_models)
        model->render();
}

