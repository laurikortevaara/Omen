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
#include "BoundingBox.h"
#include "AnimationFrame.h"
#include "Object.h"

namespace omen {
    class Mesh : public omen::Object {
    public:
        
		/**
		  Constructors and initialization
		**/

		Mesh::Mesh() : Object("Mesh") {}
		virtual ~Mesh() {};

		/**
		Data input and output and other functions
		**/
        std::vector<glm::vec3> &vertices() { return m_vertices; }
        std::vector<glm::vec3> &normals() { return m_normals; }
        std::vector<glm::vec2> &uv() { return m_uv; }
		std::vector<GLsizei> &vertexIndices() { return m_vertex_indices; }
		const BoundingBox& boundingBox() { return m_bb; }

        Mesh& setVertices(const std::vector<glm::vec3> &vertices) {
            m_vertices = vertices;
			return *this;
        }

        Mesh& setNormals(const std::vector<glm::vec3> &normals) {
			m_normals = normals;
			return *this;
        }

		Mesh& setUVs(const std::vector<glm::vec2> &uv) { 
			m_uv = uv; 
			return *this; 
		}

        Mesh& setVertexIndices(const std::vector<GLsizei> &vertexIndices) { 
			m_vertex_indices = vertexIndices; 
			return *this;
		}

		Mesh& setBoundingBox(const BoundingBox& bb) {
			m_bb = bb;
		}

	protected:
	private:
		std::vector<glm::vec3>	m_vertices;
		std::vector<glm::vec3>	m_normals;
		std::vector<glm::vec2>	m_uv;
		std::vector<GLsizei>	m_vertex_indices;
		BoundingBox				m_bb;
    };
} // namespace omen

#endif //OMEN_MESH_H
