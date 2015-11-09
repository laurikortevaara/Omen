//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_VERTEXBUFFEROBJECT_H
#define OMEN_VERTEXBUFFEROBJECT_H


#include <OpenGL/OpenGL.h>
#include "Triangle.h"
#include <memory>
#include <vector>

class VertexBufferObject {
public:
    VertexBufferObject();
    void initialize();
    void setMesh(std::vector< std::shared_ptr<Triangle> > triangles);

    void activate();
    void deactivate();

    GLuint m_index;
};


#endif //OMEN_VERTEXBUFFEROBJECT_H
