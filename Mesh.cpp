#include "Mesh.h"
#include "component/Picker.h"

using namespace omen;


Mesh::Mesh() : Object("Mesh"), m_material(nullptr) 
{
	m_bb = std::make_unique<BoundingBox>();
}


void Mesh::calcBoundingBox() {
	glm::vec3 min(10e10f), max(10e-10f);
	for (auto v : m_vertices) {
		min = glm::min(v.pos, min);
		max = glm::max(v.pos, max);
	}
	m_bb->set(min,max);

	int i = 1;
}


