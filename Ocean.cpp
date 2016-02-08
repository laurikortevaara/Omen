//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#include "Ocean.h"
#include "GL_error.h"
#include "Engine.h"
#include "component/KeyboardInput.h"


/* Constants rounded for 21 decimals. */
#define M_E 2.71828182845904523536
#define M_LOG2E 1.44269504088896340736
#define M_LOG10E 0.434294481903251827651
#define M_LN2 0.693147180559945309417
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309616
#define M_1_PI 0.318309886183790671538
#define M_2_PI 0.636619772367581343076
#define M_1_SQRTPI 0.564189583547756286948
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2 1.41421356237309504880
#define M_SQRT_2 0.707106781186547524401

using namespace omen;

float uniformRandomVariable() {
    return (float)rand()/RAND_MAX;
}

Complex gaussianRandomVariable() {
    float x1, x2, w;
    do {
        x1 = 2.f * uniformRandomVariable() - 1.f;
        x2 = 2.f * uniformRandomVariable() - 1.f;
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.f );
    w = (float) sqrt((-2.f * log(w)) / w);
    return Complex(x1 * w, x2 * w);
}

float Ocean::dispersion(int n_prime, int m_prime) {
    float w_0 = (float) (2.0f * M_PI / 200.0f);
    float kx = (float) (M_PI * (2 * n_prime - N) / length);
    float kz = (float) (M_PI * (2 * m_prime - N) / length);
    return (float) (floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0);
}

static float _dot(const glm::vec2 &a, const glm::vec2 &b) {
    return a.x * b.x + a.y * b.y;
}

static float _dot(const glm::vec3 &a, const glm::vec3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}


float Ocean::phillips(int n_prime, int m_prime) {
    glm::vec2 k(M_PI * (2 * n_prime - N) / length,
              M_PI * (2 * m_prime - N) / length);
    float k_length  = k.length();
    if (k_length < 0.000001) return 0.0;

    float k_length2 = k_length  * k_length;
    float k_length4 = k_length2 * k_length2;

    glm::vec2 kn = k;
    glm::vec2 wn = w;
    //float k_dot_w   = _dot(glm::normalize(kn), glm::normalize(wn));
    float k_dot_w   = _dot(glm::normalize(kn), glm::normalize(wn));
    float k_dot_w2  = k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w;

    float w_length  = w.length();
    float L         = w_length * w_length / g;
    float L2        = L * L;

    float damping   = 0.001;
    float l2        = L2 * damping * damping;

    return A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
}

complex_vector_normal Ocean::h_D_and_n(glm::vec2 x, float t) {
    Complex h(0.0f, 0.0f);
    glm::vec2 D(0.0f, 0.0f);
    glm::vec3 n(0.0f, 0.0f, 0.0f);

    Complex c, res, htilde_c;
    glm::vec2 k;
    float kx, kz, k_length, k_dot_x;

    for (int m_prime = 0; m_prime < N; m_prime++) {
        kz = (float) (2.0f * M_PI * (m_prime - N / 2.0f) / length);
        for (int n_prime = 0; n_prime < N; n_prime++) {
            kx = (float) (2.0f * M_PI * (n_prime - N / 2.0f) / length);
            k = glm::vec2(kx, kz);

            k_length = k.length();
            k_dot_x = _dot(k, x);

            c = Complex((float) cos(k_dot_x), (float) sin(k_dot_x));
            htilde_c = hTilde(t, n_prime, m_prime) * c;

            h = h + htilde_c;

            n = n + glm::vec3(-kx * htilde_c.b, 0.0f, -kz * htilde_c.b);

            if (k_length < 0.000001) continue;
            D = D + glm::vec2(kx / k_length * htilde_c.b, kz / k_length * htilde_c.b);
        }
    }

    n = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f) - n);

    complex_vector_normal cvn;
    cvn.h = h;
    cvn.D = D;
    cvn.n = n;
    return cvn;
}


Complex Ocean::hTilde_0(int n_prime, int m_prime) {
    Complex r = gaussianRandomVariable();
    return r * sqrt(phillips(n_prime, m_prime) / 2.0f);
}

Complex Ocean::hTilde(float t, int n_prime, int m_prime) {
    int index = m_prime * Nplus1 + n_prime;

    Complex htilde0(m_vertices[index].a, m_vertices[index].b);
    Complex htilde0mkconj(m_vertices[index]._a, m_vertices[index]._b);

    float omegat = dispersion(n_prime, m_prime) * t;

    float cos_ = (float) cos(omegat);
    float sin_ = (float) sin(omegat);

    Complex c0(cos_, sin_);
    Complex c1(cos_, -sin_);

    Complex res = htilde0 * c0 + htilde0mkconj * c1;

    return htilde0 * c0 + htilde0mkconj * c1;
}

Ocean::Ocean(int _N, float _A, const glm::vec2 &_w, const float _length) :
        Renderable({0,0,0},0,0,0),
        m_innerTesselationLevel(nullptr),
        m_outerTesselationLevel(nullptr) {

    this->g = 9.81;
    this->A = _A;
    this->N = _N;
    this->w = _w;
    this->length = _length;

    Nplus1 = N + 1;
    m_innerTessellationLevels = {1, 1};
    m_outerTessellationLevels = {1, 1, 1, 1};

    h_tilde.resize((unsigned long) (N * N));
    h_tilde_slopex.resize((unsigned long) (N * N));
    h_tilde_slopez.resize((unsigned long) (N * N));
    h_tilde_dx.resize((unsigned long) (N * N));
    h_tilde_dz.resize((unsigned long) (N * N));
    fft = new FFT(N);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);

    m_vertices.resize((unsigned long) (Nplus1 * Nplus1));

    unsigned int index;
    Complex htilde0, htilde0mk_conj;
    for (int m_prime = 0; m_prime < Nplus1; m_prime++) {
        for (int n_prime = 0; n_prime < Nplus1; n_prime++) {
            index = (unsigned int) (m_prime * Nplus1 + n_prime);

            htilde0 = hTilde_0(n_prime, m_prime);
            htilde0mk_conj = hTilde_0(-n_prime, -m_prime).conjugate();

            m_vertices[index].a = htilde0.a;
            m_vertices[index].b = htilde0.b;
            m_vertices[index]._a = htilde0mk_conj.a;
            m_vertices[index]._b = htilde0mk_conj.b;

            m_vertices[index].ox = m_vertices[index].x = (n_prime - N / 2.0f) * length / N;
            m_vertices[index].oy = m_vertices[index].y = 0.0f;
            m_vertices[index].oz = m_vertices[index].z = (m_prime - N / 2.0f) * length / N;

            m_vertices[index].nx = 0.0f;
            m_vertices[index].ny = 1.0f;
            m_vertices[index].nz = 0.0f;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_ocean) * m_vertices.size(), m_vertices.data(), GL_DYNAMIC_DRAW);

    indices_count = 0;
    m_indices.resize((unsigned long) (Nplus1 * Nplus1 * 10));
    for (int m_prime = 0; m_prime < N; m_prime++) {
        for (int n_prime = 0; n_prime < N; n_prime++) {
            index = (unsigned int) (m_prime * Nplus1 + n_prime);
            m_indices[indices_count++] = index;               // two triangles
            m_indices[indices_count++] = index + Nplus1;
            m_indices[indices_count++] = index + Nplus1 + 1;
            m_indices[indices_count++] = index;
            m_indices[indices_count++] = index + Nplus1 + 1;
            m_indices[indices_count++] = index + 1;
        }
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);

    Engine::instance()->findComponent<KeyboardInput>()->signal_key_release.connect([this](int k, int s, int a, int m) {
        if (m != GLFW_MOD_SHIFT) {
            switch (k) {
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
                    (*m_innerTesselationLevel) = (*m_innerTesselationLevel) > 1 ? (*m_innerTesselationLevel) - 1 : 1;
                }
            }

        }
        else {
            switch (k) {
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
                    (*m_outerTesselationLevel) = (*m_outerTesselationLevel) > 1 ? (*m_outerTesselationLevel) - 1 : 1;
                }
            }
        }
    });
}

Ocean::~Ocean() {
}

void Ocean::render() {
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, Engine::instance()->getPolygonMode());
    shader()->use();
    //shader()->setUniform4fv("Color", 1, &glm::vec4(0,0,0,1)[0] );
    shader()->setUniform1f("Time", (float) Engine::instance()->time());
    shader()->setUniform4fv("Color", 1, &glm::vec4(1)[0]);
    shader()->setUniformMatrix4fv("ViewMatrix", 1, &Engine::instance()->camera()->view()[0][0], false);
    shader()->setUniformMatrix3fv("NormalMatrix", 1, &glm::mat3(Engine::instance()->camera()->view())[0][0], false);
    shader()->setUniformMatrix4fv("Model", 1, &glm::mat4(1)[0][0], false);
    shader()->setUniformMatrix4fv("ModelViewProjection", 1,
                                  &(Engine::instance()->camera()->viewProjection())[0][0], false);
    shader()->setUniform1i("InnerTessellationLevel1", m_innerTessellationLevels[0]);
    shader()->setUniform1i("InnerTessellationLevel2", m_innerTessellationLevels[1]);

    shader()->setUniform1i("OuterTessellationLevel1", m_outerTessellationLevels[0]);
    shader()->setUniform1i("OuterTessellationLevel2", m_outerTessellationLevels[1]);
    shader()->setUniform1i("OuterTessellationLevel3", m_outerTessellationLevels[2]);
    shader()->setUniform1i("OuterTessellationLevel4", m_outerTessellationLevels[3]);

    glBindVertexArray(m_vao);

    /**/
    static bool b = false;
    if (!b) {
        b = false;
        evaluateWavesFFT((float) (Engine::instance()->time() * 0.5));
        //for(auto v : m_vertices)
        //    std::cout << "V: " << v.x << ", " << v.y << ", " << v.z << std::endl;

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_ocean) * Nplus1 * Nplus1, m_vertices.data());
        GLint vertex = shader()->getAttribLocation("position");
        GLint normal = shader()->getAttribLocation("normal");
        GLint texture = shader()->getAttribLocation("texcoord");
        glEnableVertexAttribArray((GLuint) vertex);
        glVertexAttribPointer((GLuint) vertex, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_ocean), 0);
        glEnableVertexAttribArray((GLuint) normal);
        glVertexAttribPointer((GLuint) normal, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_ocean), (char *) NULL + 12);
        glEnableVertexAttribArray((GLuint) texture);
        glVertexAttribPointer((GLuint) texture, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_ocean), (char *) NULL + 24);
    }


    /**/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    //for (int x = -10; x < 10; ++x)
    //        for (int z = -10; z < 10; ++z) {
            glm::mat4 Model(1);
            //Model = glm::translate(Model, glm::vec3(100 * x, 0, 100 * z));
            shader()->setUniformMatrix4fv("Model", 1, &glm::scale(Model,glm::vec3(5.f,5.f,5.f))[0][0], false);

            //glPatchParameteri(GL_PATCH_VERTICES, 3);
            //glDrawElements(GL_PATCHES, m_indices.size(), GL_UNSIGNED_SHORT, 0);
            //glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
            glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
      //  }

    /*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shader()->setUniform4fv("Color", 1, &glm::vec4(0.5,0.7,0.8,1.0)[0] );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glDrawElements(GL_TRIANGLE_STRIP, m_indices.size(), GL_UNSIGNED_SHORT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
    check_gl_error();
}

void Ocean::evaluateWaves(float t) {
    float lambda = -1.0;
    int index;
    glm::vec2 x;
    glm::vec2 d;
    complex_vector_normal h_d_and_n;
    for (int m_prime = 0; m_prime < N; m_prime++) {
        for (int n_prime = 0; n_prime < N; n_prime++) {
            index = m_prime * Nplus1 + n_prime;

            x = glm::vec2(m_vertices[index].x, m_vertices[index].z);

            h_d_and_n = h_D_and_n(x, t);

            m_vertices[index].y = h_d_and_n.h.a;

            m_vertices[index].x = m_vertices[index].ox + lambda * h_d_and_n.D.x;
            m_vertices[index].z = m_vertices[index].oz + lambda * h_d_and_n.D.y;

            m_vertices[index].nx = h_d_and_n.n.x;
            m_vertices[index].ny = h_d_and_n.n.y;
            m_vertices[index].nz = h_d_and_n.n.z;

            if (n_prime == 0 && m_prime == 0) {
                m_vertices[index + N + Nplus1 * N].y = h_d_and_n.h.a;

                m_vertices[index + N + Nplus1 * N].x = m_vertices[index + N + Nplus1 * N].ox + lambda * h_d_and_n.D.x;
                m_vertices[index + N + Nplus1 * N].z = m_vertices[index + N + Nplus1 * N].oz + lambda * h_d_and_n.D.y;

                m_vertices[index + N + Nplus1 * N].nx = h_d_and_n.n.x;
                m_vertices[index + N + Nplus1 * N].ny = h_d_and_n.n.y;
                m_vertices[index + N + Nplus1 * N].nz = h_d_and_n.n.z;
            }
            if (n_prime == 0) {
                m_vertices[index + N].y = h_d_and_n.h.a;

                m_vertices[index + N].x = m_vertices[index + N].ox + lambda * h_d_and_n.D.x;
                m_vertices[index + N].z = m_vertices[index + N].oz + lambda * h_d_and_n.D.y;

                m_vertices[index + N].nx = h_d_and_n.n.x;
                m_vertices[index + N].ny = h_d_and_n.n.y;
                m_vertices[index + N].nz = h_d_and_n.n.z;
            }
            if (m_prime == 0) {
                m_vertices[index + Nplus1 * N].y = h_d_and_n.h.a;

                m_vertices[index + Nplus1 * N].x = m_vertices[index + Nplus1 * N].ox + lambda * h_d_and_n.D.x;
                m_vertices[index + Nplus1 * N].z = m_vertices[index + Nplus1 * N].oz + lambda * h_d_and_n.D.y;

                m_vertices[index + Nplus1 * N].nx = h_d_and_n.n.x;
                m_vertices[index + Nplus1 * N].ny = h_d_and_n.n.y;
                m_vertices[index + Nplus1 * N].nz = h_d_and_n.n.z;
            }
        }
    }
}

void Ocean::evaluateWavesFFT(float t) {
    float kx, kz, len, lambda = -1.0f;
    int index, index1;

    for (int m_prime = 0; m_prime < N; m_prime++) {
        kz = M_PI * (2.0f * m_prime - N) / length;
        for (int n_prime = 0; n_prime < N; n_prime++) {
            kx = M_PI * (2 * n_prime - N) / length;
            len = sqrt(kx * kx + kz * kz);
            index = m_prime * N + n_prime;

            h_tilde[index] = hTilde(t, n_prime, m_prime);
            h_tilde_slopex[index] = h_tilde[index] * Complex(0, kx);
            h_tilde_slopez[index] = h_tilde[index] * Complex(0, kz);
            if (len < 0.000001f) {
                h_tilde_dx[index] = Complex(0.0f, 0.0f);
                h_tilde_dz[index] = Complex(0.0f, 0.0f);
            } else {
                h_tilde_dx[index] = h_tilde[index] * Complex(0, -kx / len);
                h_tilde_dz[index] = h_tilde[index] * Complex(0, -kz / len);
            }
        }
    }

    for (int m_prime = 0; m_prime < N; m_prime++) {
        fft->fft(h_tilde.data(), h_tilde.data(), 1, m_prime * N);
        fft->fft(h_tilde_slopex.data(), h_tilde_slopex.data(), 1, m_prime * N);
        fft->fft(h_tilde_slopez.data(), h_tilde_slopez.data(), 1, m_prime * N);
        fft->fft(h_tilde_dx.data(), h_tilde_dx.data(), 1, m_prime * N);
        fft->fft(h_tilde_dz.data(), h_tilde_dz.data(), 1, m_prime * N);
    }
    for (int n_prime = 0; n_prime < N; n_prime++) {
        fft->fft(h_tilde.data(), h_tilde.data(), N, n_prime);
        fft->fft(h_tilde_slopex.data(), h_tilde_slopex.data(), N, n_prime);
        fft->fft(h_tilde_slopez.data(), h_tilde_slopez.data(), N, n_prime);
        fft->fft(h_tilde_dx.data(), h_tilde_dx.data(), N, n_prime);
        fft->fft(h_tilde_dz.data(), h_tilde_dz.data(), N, n_prime);
    }

    int sign;
    float signs[] = {1.0f, -1.0f};
    glm::vec3 n;
    for (int m_prime = 0; m_prime < N; m_prime++) {
        for (int n_prime = 0; n_prime < N; n_prime++) {
            index = m_prime * N + n_prime;     // index into h_tilde..
            index1 = m_prime * Nplus1 + n_prime;    // index into m_vertices

            sign = signs[(n_prime + m_prime) & 1];

            h_tilde[index] = h_tilde[index] * sign;

            // height
            m_vertices[index1].y = h_tilde[index].a;

            // displacement
            h_tilde_dx[index] = h_tilde_dx[index] * sign;
            h_tilde_dz[index] = h_tilde_dz[index] * sign;
            m_vertices[index1].x = m_vertices[index1].ox + h_tilde_dx[index].a * lambda;
            m_vertices[index1].z = m_vertices[index1].oz + h_tilde_dz[index].a * lambda;

            // normal
            h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
            h_tilde_slopez[index] = h_tilde_slopez[index] * sign;
            n = glm::normalize(glm::vec3(0.0f - h_tilde_slopex[index].a, 1.0f, 0.0f - h_tilde_slopez[index].a));
            m_vertices[index1].nx = n.x;
            m_vertices[index1].ny = n.y;
            m_vertices[index1].nz = n.z;

            // for tiling
            if (n_prime == 0 && m_prime == 0) {
                m_vertices[index1 + N + Nplus1 * N].y = h_tilde[index].a;

                m_vertices[index1 + N + Nplus1 * N].x =
                        m_vertices[index1 + N + Nplus1 * N].ox + h_tilde_dx[index].a * lambda;
                m_vertices[index1 + N + Nplus1 * N].z =
                        m_vertices[index1 + N + Nplus1 * N].oz + h_tilde_dz[index].a * lambda;

                m_vertices[index1 + N + Nplus1 * N].nx = n.x;
                m_vertices[index1 + N + Nplus1 * N].ny = n.y;
                m_vertices[index1 + N + Nplus1 * N].nz = n.z;
            }
            if (n_prime == 0) {
                m_vertices[index1 + N].y = h_tilde[index].a;

                m_vertices[index1 + N].x = m_vertices[index1 + N].ox + h_tilde_dx[index].a * lambda;
                m_vertices[index1 + N].z = m_vertices[index1 + N].oz + h_tilde_dz[index].a * lambda;

                m_vertices[index1 + N].nx = n.x;
                m_vertices[index1 + N].ny = n.y;
                m_vertices[index1 + N].nz = n.z;
            }
            if (m_prime == 0) {
                m_vertices[index1 + Nplus1 * N].y = h_tilde[index].a;

                m_vertices[index1 + Nplus1 * N].x = m_vertices[index1 + Nplus1 * N].ox + h_tilde_dx[index].a * lambda;
                m_vertices[index1 + Nplus1 * N].z = m_vertices[index1 + Nplus1 * N].oz + h_tilde_dz[index].a * lambda;

                m_vertices[index1 + Nplus1 * N].nx = n.x;
                m_vertices[index1 + Nplus1 * N].ny = n.y;
                m_vertices[index1 + Nplus1 * N].nz = n.z;
            }
        }
    }
}

FFT::FFT(unsigned int N) : N(N), reversed(0), T(0), pi2(2 * M_PI) {
    c[0] = c[1] = 0;

    log_2_N = log(N) / log(2);

    reversed = new unsigned int[N];     // prep bit reversals
    for (int i = 0; i < N; i++) reversed[i] = reverse(i);

    int pow2 = 1;
    T = new Complex *[log_2_N];      // prep T
    for (int i = 0; i < log_2_N; i++) {
        T[i] = new Complex[pow2];
        for (int j = 0; j < pow2; j++) T[i][j] = t(j, pow2 * 2);
        pow2 *= 2;
    }

    c[0] = new Complex[N];
    c[1] = new Complex[N];
    which = 0;
}

FFT::~FFT() {
    if (c[0]) delete[] c[0];
    if (c[1]) delete[] c[1];
    if (T) {
        for (int i = 0; i < log_2_N; i++) if (T[i]) delete[] T[i];
        delete[] T;
    }
    if (reversed) delete[] reversed;
}

unsigned int FFT::reverse(unsigned int i) {
    unsigned int res = 0;
    for (int j = 0; j < log_2_N; j++) {
        res = (res << 1) + (i & 1);
        i >>= 1;
    }
    return res;
}

Complex FFT::t(unsigned int x, unsigned int N) {
    return Complex(cos(pi2 * x / N), sin(pi2 * x / N));
}

void FFT::fft(Complex *input, Complex *output, int stride, int offset) {
    for (int i = 0; i < N; i++) c[which][i] = input[reversed[i] * stride + offset];

    int loops = N >> 1;
    int size = 1 << 1;
    int size_over_2 = 1;
    int w_ = 0;
    for (int i = 1; i <= log_2_N; i++) {
        which ^= 1;
        for (int j = 0; j < loops; j++) {
            for (int k = 0; k < size_over_2; k++) {
                c[which][size * j + k] = c[which ^ 1][size * j + k] +
                                         c[which ^ 1][size * j + size_over_2 + k] * T[w_][k];
            }

            for (int k = size_over_2; k < size; k++) {
                c[which][size * j + k] = c[which ^ 1][size * j - size_over_2 + k] -
                                         c[which ^ 1][size * j + k] * T[w_][k - size_over_2];
            }
        }
        loops >>= 1;
        size <<= 1;
        size_over_2 <<= 1;
        w_++;
    }

    for (int i = 0; i < N; i++) output[i * stride + offset] = c[which][i];
}

void Ocean::initializeShader() {
    setShader(new Shader("shaders/ocean.glsl"));
}

void Ocean::initializeTexture() {
    setTexture(nullptr);
}
