//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#ifndef OMEN_OCEAN_H
#define OMEN_OCEAN_H

#include "component/MeshRenderer.h"
#include "GameObject.h"

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

    class OceanRenderer : public ecs::MeshRenderer {
    public:
        OceanRenderer(ecs::MeshController* mc);
        virtual ~OceanRenderer();

		virtual void render(Shader* shader = nullptr);

		double phillips(glm::vec2 k, float max_l);
		void generate_distribution(Complex *distribution, glm::vec2 size, float amplitude, float max_l);
        
    protected:
        virtual void initializeShader();
        virtual void initializeTexture();
		
    private:
		GLuint m_vao, m_vbo, m_vbo_texcoord, m_ssbo, m_ssbo2, m_ssbo_index, m_ssbo_index2;
    };

	class Ocean : public ecs::GameObject
	{
	public:
		Ocean();
		virtual ~Ocean();


	protected:
	private:
		
	};


} // namespace omen


#endif //OMEN_OCEAN_H
