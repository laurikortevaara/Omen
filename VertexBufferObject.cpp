//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <iostream>
#include "VertexBufferObject.h"
#include "GL_error.h"

VertexBufferObject::VertexBufferObject() {
    initialize();
    check_gl_error();
}

void VertexBufferObject::activate() {
    glBindBuffer(GL_ARRAY_BUFFER, m_index);
    std::cout << "VBO Activate" << std::endl;
    check_gl_error();
}

void VertexBufferObject::deactivate() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    std::cout << "VBO De-Activate" << std::endl;
    check_gl_error();
}

void VertexBufferObject::initialize() {
    glGenBuffers(1, &m_index);
    activate();
    if(glIsBuffer(m_index)!=GL_TRUE)
    {
        std::cout << "Error" << std::endl;
    }
    check_gl_error();
}

void VertexBufferObject::setMesh(std::vector<std::shared_ptr<Triangle> > triangles) {
    std::vector<GLfloat> verts;
    for(auto t : triangles) {
        verts.push_back(t->m_p1.x);
        verts.push_back(t->m_p1.y);
        verts.push_back(t->m_p1.z);

        verts.push_back(t->m_p2.x);
        verts.push_back(t->m_p2.y);
        verts.push_back(t->m_p2.z);

        verts.push_back(t->m_p3.x);
        verts.push_back(t->m_p3.y);
        verts.push_back(t->m_p3.z);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*verts.size(), verts.data(), GL_STATIC_DRAW);
    check_gl_error();
}
