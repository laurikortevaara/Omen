//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_MESH_H
#define OMEN_MESH_H


#include <vector>
#include "Triangle.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "Material.h"
#include "Shader.h"
#include "component/Transform.h"

namespace Omen {
    class Mesh {
    public:
        Mesh();

        virtual ~Mesh();

        Transform m_transform;

        std::vector<std::shared_ptr<Triangle> > m_triangles;
        std::shared_ptr<VertexArrayObject> m_vao;
        std::shared_ptr<VertexBufferObject> m_vbo;

        std::vector<glm::vec3> m_vertices;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec2> m_texture_coords;
        std::vector<unsigned long> m_vertex_indices;

        std::vector<glm::vec3> &vertices() { return m_vertices; }

        std::vector<glm::vec3> &normals() { return m_normals; }

        std::vector<glm::vec2> &textureCoords() { return m_texture_coords; }

        std::vector<unsigned long> &vertexIndices() {return m_vertex_indices;}

        void setVertices(const std::vector<glm::vec3> &vertices) { m_vertices = vertices; }

        void setNormals(const std::vector<glm::vec3> &normals) { m_normals = normals; }

        void setTextureCoords(const std::vector<glm::vec2> &texCoords) { m_texture_coords = texCoords; }
        void setVertexIndices(const std::vector<unsigned long>& vertexIndices) {m_vertex_indices = vertexIndices;}

        void createMesh();

        void createPatches();

        float fOuterTess, fInnerTess;

        void loadTextures();

        GLenum mPolygonMode;
        GLint m_use_texture;

        Shader *m_shader;
        Material *m_material;

        Shader *shader() { return m_shader; }

        void setShader(Shader *shader) { m_shader = shader; }

        Material *material() { return m_material; }

        void setMaterial(Material *material) { m_material = material; }

        void render(const glm::mat4 &viewProjection);
    };
} // namespace Omen

#endif //OMEN_MESH_H
