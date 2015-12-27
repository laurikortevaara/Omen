//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <OpenGL/gl.h>
#include "Model.h"
#include "Shader.h"
#include <memory>

using namespace Omen;

Model::Model() {
    m_mesh = std::make_shared<Mesh>(Mesh());
}

void Model::render(const glm::mat4 &viewProjection) {
    m_mesh->render(viewProjection);
}

