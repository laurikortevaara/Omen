//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_MESH_H
#define OMEN_MESH_H


#include <vector>
#include <map>
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
/*
 * // C++11 Move-ctor
        Mesh(Mesh&& other) {
            int i=1;
        }
        // Copy-ctor
        Mesh(const Mesh& other){}

        // Copy assignment operator
        Mesh& operator=(const Mesh& other) {
            return *this;
        }

        // C++11 Move-ctor
        Mesh(Mesh&& other) {
        }

        // C++11 Move assignment operator
        Mesh& operator=(Mesh&& other)  {
            if(this != &other){
            }
            return *this;
        }

        Mesh& operator=(const Mesh&& other) {
            return *this;
        }
*/
        Mesh(const std::string& shader,
             Material* material,
             std::vector<glm::vec3>& vertices,
             std::vector<glm::vec3>& normals,
             std::vector<glm::vec2>& texcoords,
             std::vector<GLsizei> indices);

        virtual ~Mesh();

        Transform m_transform;

        std::vector<std::shared_ptr<Triangle> > m_triangles;

        std::vector<glm::vec3> m_vertices;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec2> m_texture_coords;
        std::vector<GLsizei> m_vertex_indices;

        std::vector<glm::vec3> &vertices() { return m_vertices; }

        std::vector<glm::vec3> &normals() { return m_normals; }

        std::vector<glm::vec2> &textureCoords() { return m_texture_coords; }

        std::vector<GLsizei> &vertexIndices() {return m_vertex_indices;}

        void setVertices(const std::vector<glm::vec3> &vertices) { m_vertices = vertices; }

        void setNormals(const std::vector<glm::vec3> &normals) { m_normals = normals; }

        void setTextureCoords(const std::vector<glm::vec2> &texCoords) { m_texture_coords = texCoords; }
        void setVertexIndices(const std::vector<GLsizei>& vertexIndices) {m_vertex_indices = vertexIndices;}

        void createMesh();

        void createPatches();

        float fOuterTess, fInnerTess;

        void loadTextures();

        GLenum mPolygonMode;
        GLint m_use_texture;

        static std::map<std::string, Shader*> shaders;
        Shader *m_shader;
        Material *m_material;
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_vbo_normals;
        GLuint m_vbo_texcoords;
        GLuint m_ibo;

        Texture *m_texture;
        Texture *m_texture2;

        GLint m_vertex_position_attrib;
        GLint m_vertex_texture_coord_attrib;
        GLint m_vertex_normals_attrib;

        double m_amplitude;
        double m_phase;
        double m_frequency;
        Shader *shader() { return m_shader; }

        void setShader(Shader *shader) { m_shader = shader; }

        Material *material() { return m_material; }

        void setMaterial(Material *material) { m_material = material; }

        void render(const glm::mat4 &viewProjection, const glm::mat4 &view);

        glm::vec3 m_position;

        void createTextureCoordBuffer(GLint texcoord_attrib, std::vector<glm::vec2> &texcoords);

        void createVertexCoordBuffer(GLint vcoord_attrib, std::vector<glm::vec3> &vertices);

        void createIndexBuffer(std::vector<GLsizei> &indices);

        std::string getDefaultTexture();

        void createShader(const std::string &shaderName);

        void initialize();

        void createVertexNormalBuffer(GLint vnormal_attrib, std::vector<glm::vec3> &normals);

        void genBuffers();

        void create(const std::string &shader, Material *material, std::vector<glm::vec3> &vertices,
                std::vector<glm::vec3> &normals, std::vector<glm::vec2> &texcoords, std::vector<GLsizei> indices);
    };
} // namespace Omen

#endif //OMEN_MESH_H
