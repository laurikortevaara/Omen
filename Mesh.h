//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_MESH_H
#define OMEN_MESH_H


#include <vector>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include "Triangle.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

class Mesh {
public:
    Mesh();

    virtual ~Mesh();

    void render();

    std::vector<std::shared_ptr<Triangle> > m_triangles;
    std::shared_ptr<VertexArrayObject> m_vao;
    std::shared_ptr<VertexBufferObject> m_vbo;

    void loadShaders();

    void createMesh();

    void createPatches();

    float fInnerTess, fOuterTess;

    void loadTextures();

    GLenum mPolygonMode;
    GLint m_use_texture;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

};


#endif //OMEN_MESH_H
