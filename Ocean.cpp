//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#include "Ocean.h"
#include "GL_error.h"
#include "Engine.h"
#include "component/KeyboardInput.h"

using namespace Omen;

Ocean::Ocean() :
        m_innerTesselationLevel(nullptr),
        m_outerTesselationLevel(nullptr){

    m_innerTessellationLevels = {1,1};
    m_outerTessellationLevels = {1,1,1,1};

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);

    m_shader = new Shader("shaders/ocean.glsl");

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    float s = 100;
    std::vector<glm::vec3> vertices;
    int n = s/2;
    float num = pow(n, 2);
    float xn = sqrt(num);
    float yn = sqrt(num);
    for (int y = 0; y < yn; ++y) {
        for (int x = 0; x < xn; ++x) {
            float dx = s / xn;
            float dy = s / yn;
            float x1 = -s / 2 + dx * x;
            float x2 = x1 + dx - dx * 0.0;
            float y1 = -s / 2 + dy * (y);
            float y2 = y1 + dy - dy * 0.0;

            vertices.push_back({x1, 0.01, y2});
            vertices.push_back({x1, 0.01, y1});
            vertices.push_back({x2, 0.01, y2});
            vertices.push_back({x2, 0.01, y1});
        }
    }

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                          0/*stride*/, 0/*offset*/);

    for (int i = 0; i < num; ++i) {
        m_indices.push_back(0 + i * 4);
        m_indices.push_back(1 + i * 4);
        m_indices.push_back(2 + i * 4);
        m_indices.push_back(1 + i * 4);
        m_indices.push_back(3 + i * 4);
        m_indices.push_back(2 + i * 4);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLshort), m_indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);

    Engine::instance()->findComponent<KeyboardInput>()->signal_key_release.connect([this](int k, int s, int a, int m) {
        if (m != GLFW_MOD_SHIFT) {
            switch(k){
                case GLFW_KEY_0 :
                    m_innerTesselationLevel = nullptr;
                    break;
                case GLFW_KEY_1 :
                    m_innerTesselationLevel = &m_innerTessellationLevels[0];
                    break;
                case GLFW_KEY_2 :
                    m_innerTesselationLevel = &m_innerTessellationLevels[1];
                    break;
            }
            if (k == GLFW_KEY_KP_ADD) {
                if (m_innerTesselationLevel == nullptr) {
                    for (auto &level : m_innerTessellationLevels) {
                        level++;
                    }
                }
                else {
                    (*m_innerTesselationLevel)++;
                }

            }

            if (k == GLFW_KEY_KP_SUBTRACT) {
                if (m_innerTesselationLevel == nullptr) {
                    for (auto &level : m_innerTessellationLevels) {
                        level = level > 1 ? level - 1 : 1;
                    }
                }
                else {
                    (*m_innerTesselationLevel) = (*m_innerTesselationLevel) > 1 ? (*m_innerTesselationLevel) -1 : 1;
                }
            }

        }
        else {
            switch(k){
                case GLFW_KEY_0 :
                    m_outerTesselationLevel = nullptr;
                    break;
                case GLFW_KEY_1 :
                    m_outerTesselationLevel = &m_outerTessellationLevels[0];
                    break;
                case GLFW_KEY_2 :
                    m_outerTesselationLevel = &m_outerTessellationLevels[1];
                    break;
                case GLFW_KEY_3 :
                    m_outerTesselationLevel = &m_outerTessellationLevels[2];
                    break;
                case GLFW_KEY_4 :
                    m_outerTesselationLevel = &m_outerTessellationLevels[3];
                    break;
            }
            if (k == GLFW_KEY_KP_ADD) {
                if (m_outerTesselationLevel == nullptr) {
                    for (auto &level : m_outerTessellationLevels) {
                        level++;
                    }
                }
                else {
                    (*m_outerTesselationLevel)++;
                }

            }

            if (k == GLFW_KEY_KP_SUBTRACT) {
                if (m_outerTesselationLevel == nullptr) {
                    for (auto &level : m_outerTessellationLevels) {
                        level = level > 1 ? level - 1 : 1;
                    }
                }
                else {
                    (*m_outerTesselationLevel) = (*m_outerTesselationLevel) > 1 ? (*m_outerTesselationLevel) -1 : 1;
                }
            }
        }
    });
}

/*
void Ocean::evaluateWaves(float t) {
    float lambda = -1.0;
    int index;
    vector2 x;
    vector2 d;
    complex_vector_normal h_d_and_n;
    for (int m_prime = 0; m_prime < N; m_prime++) {
        for (int n_prime = 0; n_prime < N; n_prime++) {
            index = m_prime * Nplus1 + n_prime;

            x = vector2(vertices[index].x, vertices[index].z);

            h_d_and_n = h_D_and_n(x, t);

            vertices[index].y = h_d_and_n.h.a;

            vertices[index].x = vertices[index].ox + lambda*h_d_and_n.D.x;
            vertices[index].z = vertices[index].oz + lambda*h_d_and_n.D.y;

            vertices[index].nx = h_d_and_n.n.x;
            vertices[index].ny = h_d_and_n.n.y;
            vertices[index].nz = h_d_and_n.n.z;

            if (n_prime == 0 && m_prime == 0) {
                vertices[index + N + Nplus1 * N].y = h_d_and_n.h.a;

                vertices[index + N + Nplus1 * N].x = vertices[index + N + Nplus1 * N].ox + lambda*h_d_and_n.D.x;
                vertices[index + N + Nplus1 * N].z = vertices[index + N + Nplus1 * N].oz + lambda*h_d_and_n.D.y;

                vertices[index + N + Nplus1 * N].nx = h_d_and_n.n.x;
                vertices[index + N + Nplus1 * N].ny = h_d_and_n.n.y;
                vertices[index + N + Nplus1 * N].nz = h_d_and_n.n.z;
            }
            if (n_prime == 0) {
                vertices[index + N].y = h_d_and_n.h.a;

                vertices[index + N].x = vertices[index + N].ox + lambda*h_d_and_n.D.x;
                vertices[index + N].z = vertices[index + N].oz + lambda*h_d_and_n.D.y;

                vertices[index + N].nx = h_d_and_n.n.x;
                vertices[index + N].ny = h_d_and_n.n.y;
                vertices[index + N].nz = h_d_and_n.n.z;
            }
            if (m_prime == 0) {
                vertices[index + Nplus1 * N].y = h_d_and_n.h.a;

                vertices[index + Nplus1 * N].x = vertices[index + Nplus1 * N].ox + lambda*h_d_and_n.D.x;
                vertices[index + Nplus1 * N].z = vertices[index + Nplus1 * N].oz + lambda*h_d_and_n.D.y;

                vertices[index + Nplus1 * N].nx = h_d_and_n.n.x;
                vertices[index + Nplus1 * N].ny = h_d_and_n.n.y;
                vertices[index + Nplus1 * N].nz = h_d_and_n.n.z;
            }
        }
    }
}*/


Ocean::~Ocean() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);
    delete m_shader;
}

void Ocean::render() {
    glPolygonMode(GL_FRONT_AND_BACK, Engine::instance()->getPolygonMode());
    m_shader->use();
    //m_shader->setUniform4fv("Color", 1, &glm::vec4(0,0,0,1)[0] );
    m_shader->setUniform1f("Time", Engine::instance()->time());
    m_shader->setUniform4fv("Color", 1, &glm::vec4(1)[0]);
    m_shader->setUniformMatrix4fv("ViewMatrix", 1, &Engine::instance()->camera()->view()[0][0], false);
    m_shader->setUniformMatrix3fv("NormalMatrix", 1, &glm::mat3(Engine::instance()->camera()->view())[0][0], false);
    m_shader->setUniformMatrix4fv("Model", 1, &glm::mat4(1)[0][0], false);
    m_shader->setUniformMatrix4fv("ModelViewProjection", 1,
                                  &(Engine::instance()->camera()->viewProjection() * glm::mat4(1))[0][0], false);
    m_shader->setUniform1i("InnerTessellationLevel1", m_innerTessellationLevels[0]);
    m_shader->setUniform1i("InnerTessellationLevel2", m_innerTessellationLevels[1]);

    m_shader->setUniform1i("OuterTessellationLevel1", m_outerTessellationLevels[0]);
    m_shader->setUniform1i("OuterTessellationLevel2", m_outerTessellationLevels[1]);
    m_shader->setUniform1i("OuterTessellationLevel3", m_outerTessellationLevels[2]);
    m_shader->setUniform1i("OuterTessellationLevel4", m_outerTessellationLevels[3]);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    glDrawElements(GL_PATCHES, m_indices.size(), GL_UNSIGNED_SHORT, 0);

    /*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_shader->setUniform4fv("Color", 1, &glm::vec4(0.5,0.7,0.8,1.0)[0] );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glDrawElements(GL_TRIANGLE_STRIP, m_indices.size(), GL_UNSIGNED_SHORT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
    check_gl_error();
}
