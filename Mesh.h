//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_MESH_H
#define OMEN_MESH_H


#include <vector>
#include "Triangle.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

namespace Omen {
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

        float fOuterTess, fInnerTess;

        void loadTextures();

        GLenum mPolygonMode;
        GLint m_use_texture;
    };
} // namespace Omen

#endif //OMEN_MESH_H
