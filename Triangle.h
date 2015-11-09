//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_TRIANGLE_H
#define OMEN_TRIANGLE_H


#include "Vertex.h"

class Triangle {
public:
    Triangle(Vertex p1, Vertex p2, Vertex p3) :m_p1(p1), m_p2(p2), m_p3(p3) {};
    Triangle(Triangle const& triangle) {m_p1=triangle.m_p1; m_p2=triangle.m_p2; m_p3 = triangle.m_p3;}
    virtual ~Triangle();
    void render();
    Vertex m_p1,m_p2,m_p3;
};


#endif //OMEN_TRIANGLE_H
