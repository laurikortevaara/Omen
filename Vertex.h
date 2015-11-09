//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_VERTEX_H
#define OMEN_VERTEX_H


class Vertex {
public:
    Vertex() : x(0.0f), y(0.0f), z(0.0f) {};
    Vertex(float _x, float _y, float _z) : x(_x),y(_y),z(_z){}
    float x, y, z;
};


#endif //OMEN_VERTEX_H
