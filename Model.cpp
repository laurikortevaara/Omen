//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifdef _WIN32
#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif __APPLE__
#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <glfw/glfw3.h>
#endif
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

