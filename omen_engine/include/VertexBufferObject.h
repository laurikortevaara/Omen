//
// Created by Lauri Kortevaara on 08/11/15.
//

#ifndef OMEN_VERTEXBUFFEROBJECT_H
#define OMEN_VERTEXBUFFEROBJECT_H


#include "Omen_OpenGL.h"

#include "Triangle.h"
#include <memory>
#include <vector>

class VertexBufferObject {
public:
    VertexBufferObject();
    void initialize();
    void setMesh(std::vector< std::unique_ptr<Triangle> > triangles);

    void activate();
    void deactivate();

    GLuint m_index;
};


#endif //OMEN_VERTEXBUFFEROBJECT_H
