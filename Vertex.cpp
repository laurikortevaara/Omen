//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include "Vertex.h"

Vertex::Vertex(const Vertex &other) {
    m_position = other.m_position;
    m_normal = other.m_normal;

    if(!m_triangles.empty())
    for(auto t : other.m_triangles)
        m_triangles.insert(t);
}

Vertex& Vertex::operator=(Vertex const &other) {
    m_position = other.m_position;
    m_normal = other.m_normal;

    if(!m_triangles.empty())
    for(auto t : other.m_triangles)
        m_triangles.insert(t);

    return *this;
}
