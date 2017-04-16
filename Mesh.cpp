#include "Mesh.h"
#include "component/Picker.h"

using namespace omen;


Mesh::Mesh() : Object("Mesh") {
}


void Mesh::calcBoundingBox() {
	glm::vec3 min(10e10), max(10e-10);
	for (auto v : m_vertices) {
		min = glm::min(v.pos, min);
		max = glm::max(v.pos, max);
	}
	m_bb.set(min,max);
}


