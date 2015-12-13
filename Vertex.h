//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_VERTEX_H
#define OMEN_VERTEX_H

#include <glm/glm.hpp>
#include <set>

class Triangle;

class Vertex {
public:
    Vertex() : m_position(0,0,0), m_normal(0,0,0) {};
    Vertex(const Vertex& other);
    Vertex(float x, float y, float z) : m_position(x,y,z) {};

    Vertex& operator=(const Vertex& other);

    std::set<Triangle*> m_triangles;
    glm::vec3 m_position;
    glm::vec3 m_normal;
};


#endif //OMEN_VERTEX_H
