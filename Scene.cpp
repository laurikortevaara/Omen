//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include "Scene.h"
#include "Model.h"


Scene::Scene() {
    m_models.push_back(std::make_shared<Model>(Model()));
}

Scene::~Scene() {
    m_models.clear();
}

void Scene::render() {
    for(const auto & model : m_models)
        model->render();
}

