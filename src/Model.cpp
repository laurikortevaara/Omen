//
// Created by Lauri Kortevaara on 08/11/15.
//
#include "Model.h"

#include "Shader.h"

#include "GL_error.h"
#include "Omen_OpenGL.h"

#include <memory>

using namespace omen;

Model::Model(std::unique_ptr<Mesh> mesh) :
    m_mesh(std::move(mesh))
{
    std::cout << "Model::Model(Mesh*) == " << mesh.get() << std::endl;
    check_gl_error();
    if(m_mesh == nullptr)
        m_mesh = std::unique_ptr<Mesh>();
    check_gl_error();
}

void Model::render(const glm::mat4 &viewProjection, const glm::mat4 &view) {
    //m_mesh->render(viewProjection, view);
}

