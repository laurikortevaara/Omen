//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_VERTEXARRAYOBJECT_H
#define OMEN_VERTEXARRAYOBJECT_H

#include <OpenGL/OpenGL.h>

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
