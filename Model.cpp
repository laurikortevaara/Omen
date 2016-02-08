//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <OpenGL/gl.h>
#include "Model.h"
#include "Shader.h"
#include "GL_error.h"
#include <memory>

using namespace omen;

Model::Model(std::shared_ptr<Mesh> mesh) :
    m_mesh(mesh) //std::move(mesh))
{
    std::cout << "Model::Model(Mesh*) == " << mesh << std::endl;
    check_gl_error();
    if(m_mesh == nullptr)
        m_mesh = std::shared_ptr<Mesh>();
    check_gl_error();
}

void Model::render(const glm::mat4 &viewProjection, const glm::mat4 &view) {
    m_mesh->render(viewProjection, view);
}

