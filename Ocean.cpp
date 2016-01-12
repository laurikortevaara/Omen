//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#include "Ocean.h"
#include "GL_error.h"
#include "Engine.h"

using namespace Omen;

Ocean::Ocean() {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);

    m_shader = new Shader("shaders/wireframe.glsl");

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    float s = 10;
    std::vector<glm::vec3> vertices;
    float num = 4;
    float xn = sqrt(num);
    float yn = sqrt(num);
    for (int y = 0; y < yn; ++y) {
        for (int x = 0; x < xn; ++x) {
            float x1 = -s / 2 + x * s / xn;
            float x2 = x1 + (x + 1) * s / xn;
            float y1 = -s / 2 + y * s / xn;
            float y2 = y1 + (y + 1) * s / yn;

            vertices.push_back({x1, 0.01, y1});
            vertices.push_back({x1, 0.01, y2});
            vertices.push_back({x2, 0.01, y1});
            vertices.push_back({x2, 0.01, y2});
        }
    }


    glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                          0/*stride*/, 0/*offset*/);

    m_indices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLshort), m_indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}


Ocean::~Ocean() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);
    delete m_shader;
}

void Ocean::render() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_shader->use();
    m_shader->setUniformMatrix4fv("ViewMatrix", 1, &glm::inverse(Engine::instance()->camera()->view())[0][0], false);
    m_shader->setUniformMatrix4fv("Model", 1, &glm::mat4(1)[0][0], false);
    m_shader->setUniformMatrix4fv("ModelViewProjection", 1,
                                  &(Engine::instance()->camera()->viewProjection() * glm::mat4(1))[0][0], false);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/, 0/*stride*/, 0/*offset*/);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
