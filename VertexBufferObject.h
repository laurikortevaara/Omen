//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_VERTEXBUFFEROBJECT_H
#define OMEN_VERTEXBUFFEROBJECT_H


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
