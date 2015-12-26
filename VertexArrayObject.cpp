//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#define GL3_PROTOTYPES

#include <OpenGL/gl3.h>
#include <ostream>
#include <iostream>
#include "VertexArrayObject.h"
#include "GL_error.h"


VertexArrayObject::VertexArrayObject() :
m_index(0)
{
    initialize();
}

void VertexArrayObject::initialize() {
    m_index = 0;
    GLint va;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &va);

    glGenVertexArrays(1, &m_index);

    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &va);

    GLint error = glGetError();
    if(error!=GL_NO_ERROR)
    {
        std::cout << "Error while creating new vertex array: " << error << std::endl;
    }
    activate();
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &va);

    if(glIsVertexArray(m_index))
    {
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &va);
        GLint max_vertex_attribs;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &va);
        std::cout << "GL_MAX_VERTEX_ATTRIBS: " << max_vertex_attribs << std::endl;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &va);
    }
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &va);
}

VertexArrayObject::~VertexArrayObject() {
    if(glIsVertexArray(m_index))
        glDeleteVertexArrays(1,&m_index);
    check_gl_error();
}

void VertexArrayObject::activate() {
    glBindVertexArray(m_index);
    std::cout << "VAO Activate" << std::endl;
    std::cout.flush();
    check_gl_error();
}

void VertexArrayObject::deactivate() {
    glBindVertexArray(0);
    std::cout << "VAO De-Activate" << std::endl;
    check_gl_error();
}
