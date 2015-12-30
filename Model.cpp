//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <OpenGL/gl.h>
#include "Model.h"
#include "Shader.h"
#include "GL_error.h"
#include <memory>

using namespace Omen;

Model::Model(std::unique_ptr<Mesh> mesh) :
    m_mesh(std::move(mesh))
{
    check_gl_error();
    if(m_mesh == nullptr)
        m_mesh = std::make_unique<Mesh>();
    check_gl_error();
}

void Model::render(const glm::mat4 &viewProjection) {
    m_mesh->render(viewProjection);
}

