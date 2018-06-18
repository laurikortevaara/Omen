#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace omen {
	class AnimationFrame {
	public:
		std::vector<glm::vec3> m_vertices;
		std::vector<glm::vec3> m_normals;
		GLuint m_vbo;
		GLuint m_vbo_normals;
	};
}