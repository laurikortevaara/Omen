//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#ifndef OMEN_OCEAN_H
#define OMEN_OCEAN_H

#include "component/MeshRenderer.h"
#include "GameObject.h"
#include "Complex.h"
#include "FFT.h"

namespace omen {
		
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

		virtual void render(omen::Shader* shader = nullptr) const;

		double phillips(glm::vec2 k, float max_l);
        
    protected:
        virtual void initializeShader();
        virtual void initializeTexture();
		
		void compute_ocean_mapping();
		void compute_htilde0();
		void compute_fft(float t);
		void createNoiseTextures();

    private:
		GLuint m_vao, m_vbo, m_vbo_texcoord, m_ssbo, m_ssbo_index, m_ssbo_index2, m_vbo_indices;
		GLuint m_ssbo_htilde0, m_ssbo_htildes;
		GLfloat m_innerTessellationLevels[2];
		GLfloat m_outerTessellationLevels[4];
		Texture* m_foam_texture;
		GLfloat m_perlinSize;
		GLfloat m_perlinScale;

		GLuint m_noiseTexture[4];
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
