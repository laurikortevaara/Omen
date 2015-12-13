//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_MESH_H
#define OMEN_MESH_H


#include <vector>
#include "Triangle.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

class Mesh {
public:
    Mesh();
    virtual ~Mesh();
    void render();

    std::vector< Triangle* > m_triangles;
    //std::shared_ptr<VertexArrayObject> m_vao;
    //std::shared_ptr<VertexBufferObject> m_vbo;
    GLuint m_vao;
    GLuint m_vbo_vertices;
    GLuint m_vbo_normals;
    GLuint m_ibo;

    void loadShaders();

    void createMesh();

    void createPatches();

    float fOuterTess, fInnerTess;

    void loadTextures();

    GLenum mPolygonMode;
    GLint m_use_texture;

    void setupModelView();

    void createIcosahedron(std::vector<Vertex*> &mesh);

    std::vector<unsigned int> m_indices;
    std::vector<Vertex*> m_vertices;

    bool loadModel(const char *filename, std::vector<Vertex *> &mesh);
};


#endif //OMEN_MESH_H
