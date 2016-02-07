//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_VERTEXARRAYOBJECT_H
#define OMEN_VERTEXARRAYOBJECT_H

#ifdef _WIN32
#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif __APPLE__
#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

class VertexArrayObject {
public:
    VertexArrayObject();
    VertexArrayObject(VertexArrayObject const& vao){m_index = vao.m_index;}
    virtual ~VertexArrayObject();

    VertexArrayObject& operator=(VertexArrayObject const& other) {m_index = other.m_index; return *std::move(this);}
    void activate();
    void deactivate();

    void initialize();

    GLuint m_index;
};


#endif //OMEN_VERTEXARRAYOBJECT_H
