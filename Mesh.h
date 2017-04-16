//
// Created by Lauri Kortevaara on 08/11/15.
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
		class Vertex {
		public:
			Vertex(const glm::vec3& pos) :pos(pos) {}
			operator glm::vec3() { return pos; }
			operator glm::vec3&() { return pos; }
			operator Mesh::Vertex&() { return *this; }

			glm::vec3 pos;
			glm::vec3 normal;
			glm::vec3 tangent;
			glm::vec2 uv;
		};
        
		/**
		  Constructors and initialization
		**/

		Mesh::Mesh();
		virtual ~Mesh() {};

		/**
		Data input and output and other functions
		**/
        std::vector<Mesh::Vertex> &vertices() { return m_vertices; }
        std::vector<glm::vec3> &normals() { return m_normals; }
		std::vector<glm::vec3> &tangents() { return m_tangents; }
		std::vector<glm::vec3> &bitangents() { return m_bitangents; }
        std::vector<glm::vec2> &uv() { return m_uv; }
		std::vector<GLsizei> &vertexIndices() { return m_vertex_indices; }
		const BoundingBox& boundingBox() { return m_bb; }
		
		const Material* material() const { return m_material.get(); }
		void setMaterial(std::unique_ptr<Material> material) { m_material = std::move(material); }

        Mesh& setVertices(const std::vector<glm::vec3> &vertices) {
            for(auto v : vertices)
				m_vertices.push_back(v);
			return *this;
        }

		Mesh& setVertices(const std::vector<Mesh::Vertex> &vertices) {
			m_vertices = vertices;
			return *this;
		}

        Mesh& setNormals(const std::vector<glm::vec3> &normals) {
			m_normals = normals;
			return *this;
        }

		Mesh& setTangents(const std::vector<glm::vec3> &tangents) {
			m_tangents = tangents;
			return *this;
		}

		Mesh& setBiTangents(const std::vector<glm::vec3> &bitangents) {
			m_bitangents = bitangents;
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

		void calcBoundingBox();

		Mesh& setBoundingBox(const BoundingBox& bb) {
			m_bb = bb;
		}
	protected:
	private:
		std::vector<Mesh::Vertex>	m_vertices;
		//std::vector<glm::vec3>	m_vertices;
		std::vector<glm::vec3>	m_normals;
		std::vector<glm::vec3>	m_tangents;
		std::vector<glm::vec3>	m_bitangents;
		std::vector<glm::vec2>	m_uv;
		std::vector<GLsizei>	m_vertex_indices;
		BoundingBox				m_bb;
		std::unique_ptr<Material>m_material;
    };
} // namespace omen

#endif //OMEN_MESH_H
