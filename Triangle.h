//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_TRIANGLE_H
#define OMEN_TRIANGLE_H


#include <glm/glm.hpp>
#include "Vertex.h"

class Triangle : std::enable_shared_from_this<Triangle> {
public:
    Triangle(Vertex* p1, Vertex* p2, Vertex* p3);
    Triangle(Triangle const& triangle) {m_p1=triangle.m_p1; m_p2=triangle.m_p2; m_p3 = triangle.m_p3;}

    //std::shared_ptr<Triangle> ptr();

    void render();

    glm::vec3 normal() const;
    Vertex *m_p1, *m_p2, *m_p3;
    glm::vec3 m_normal;
};


#endif //OMEN_TRIANGLE_H
