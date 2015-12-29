//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <iostream>
#include "Scene.h"
#include "Model.h"
#include "utils.h"

using namespace Omen;

Scene::Scene() {
    for (int i = 0; i < 1000; ++i) {
        std::cout << "Creating model: " << i << std::endl;
        std::shared_ptr<Model> m = std::make_shared<Model>(Model());
        m->m_mesh->m_position = glm::vec3(Omen::random(-40,40), Omen::random(0,0), Omen::random(-40,40) );
        m_models.push_back(m);
    }

}

Scene::~Scene() {
    m_models.clear();
}

void Scene::render(const glm::mat4 &viewProjection) {
    for (const auto &model : m_models)
        model->render(viewProjection);
}

