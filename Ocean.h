//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#ifndef OMEN_OCEAN_H
#define OMEN_OCEAN_H

#include "Renderable.h"

namespace omen {

    class Complex {
    public:
        float a;
        float b;

        Complex() : a(0), b(0) {};
        Complex(float _a, float _b) : a(_a), b(_b) {};

        Complex operator*(float c) {
            return Complex(a*c,b*c);
        }

        Complex operator*(const Complex &r) {
            return  Complex(a*r.a-b*r.b, a*r.b+b*r.a);
        }

        Complex operator+(const Complex &r) {
            return Complex(a+r.a, b+r.b);
        }

        Complex operator-(const Complex &r) {
            return Complex(a-r.a, b-r.b);
        }

        Complex conjugate() {
            return Complex(a,-b);
        }
    };

    class FFT {
    private:
        unsigned int N, which;
        unsigned int log_2_N;
        float pi2;
        unsigned int *reversed;
        Complex **T;
        Complex *c[2];
    protected:
    public:
        FFT(unsigned int N);
        ~FFT();
        unsigned int reverse(unsigned int i);
        Complex t(unsigned int x, unsigned int N);
        void fft(Complex* input, Complex* output, int stride, int offset);
    };

    struct vertex_ocean {
        GLfloat x, y, z; // vertex
        GLfloat nx, ny, nz; // normal
        GLfloat a, b, c; // htilde0
        GLfloat _a, _b, _c; // htilde0mk conjugate
        GLfloat ox, oy, oz; // original position
    };

    struct complex_vector_normal {  // structure used with discrete fourier transform
        Complex h;      // wave height
        glm::vec2 D;      // displacement
        glm::vec3 n;      // normal
    };

    class Ocean : public Renderable {
    public:
        Ocean(int N, float A, const glm::vec2& w, const float length);

        virtual ~Ocean();
        Ocean(const Ocean& o) : Renderable({0,0,0},0,0,0){}
        Ocean& operator=(const Ocean& o) {return *this;}

        virtual void render();

        complex_vector_normal h_D_and_n(glm::vec2 x, float t);

        void evaluateWaves(float t);
        void evaluateWavesFFT(float t);

        int *m_innerTesselationLevel;
        int *m_outerTesselationLevel;
        std::vector<int> m_innerTessellationLevels;
        std::vector<int> m_outerTessellationLevels;


        float g;                                // gravity constant
        int N, Nplus1;                          // dimension -- N should be a power of 2
        float A;                                // phillips spectrum parameter -- affects heights of waves
        glm::vec2 w;                              // wind parameter
        float length;// length parameter
        std::vector<Complex> h_tilde;                       // for fast fourier transform
        std::vector<Complex> h_tilde_slopex;
        std::vector<Complex> h_tilde_slopez;
        std::vector<Complex> h_tilde_dx;
        std::vector<Complex> h_tilde_dz;
        FFT* fft;                              // fast fourier transform
        std::vector<vertex_ocean> m_vertices;

        float dispersion(int n_prime, int m_prime);

        float phillips(int n_prime, int m_prime);

        Complex hTilde_0(int n_prime, int m_prime);
        Complex hTilde(float t, int n_prime, int m_prime);

        int indices_count;
    protected:
        virtual void initializeShader();
        virtual void initializeTexture();
    private:
    };

} // namespace omen


#endif //OMEN_OCEAN_H
