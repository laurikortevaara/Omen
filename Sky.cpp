//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#include "Sky.h"
#include "GL_error.h"
#include "Engine.h"

using namespace omen;

Sky::Sky() :
    Renderable({0,0,0},0,0,0){
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);

    m_shader = new Shader("shaders/sky.glsl");

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    std::vector<glm::vec3> vertices = {
            {-1,-1,0}, {1,-1,0}, {-1,1,0}, {1,1,0}
    };
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*3*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW );
    glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                          0/*stride*/, 0/*offset*/);

    m_indices = {0,1,2,3};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(GLshort), m_indices.data(), GL_STATIC_DRAW );
    glBindVertexArray(0);
}


Sky::~Sky() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);
    delete m_shader;
}

void Sky::render() {
    m_shader->use();
    m_shader->setUniformMatrix4fv("ViewMatrix", 1, &glm::inverse(Engine::instance()->camera()->view())[0][0], false);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/, 0/*stride*/, 0/*offset*/);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0 );
    glClear(GL_DEPTH_BUFFER_BIT);
}
