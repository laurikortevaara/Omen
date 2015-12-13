//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <glm/detail/type_vec3.hpp>
#include <glm/detail/func_geometric.hpp>
#include "Triangle.h"

void Triangle::render() {

}

glm::vec3 Triangle::normal() const {
    glm::vec3 v1 = m_p1->m_position-m_p2->m_position;
    glm::vec3 v2 = m_p1->m_position-m_p3->m_position;
    return glm::normalize(glm::cross(v1,v2));
}

Triangle::Triangle(Vertex* p1, Vertex* p2, Vertex* p3) {
    m_p1 = p1;
    m_p2 = p2;
    m_p3 = p3;

    m_normal = normal();
}
