//
// Created by Lauri Kortevaara on 11/01/16.
//

#ifndef OMEN_SKY_RENDERER_H
#define OMEN_SKY_RENDERER_H

#include "Omen_OpenGL.h"
#include <vector>
#include "Shader.h"
#include "component/MeshRenderer.h"

namespace omen {
	namespace ecs {
		class MeshController;

		class SkyRenderer : public MeshRenderer {
		public:
			SkyRenderer(MeshController*);
			virtual ~SkyRenderer();

			virtual void render(omen::Shader* shader = nullptr) const;

		protected:
			virtual void initializeShader() {};
			virtual void initializeTexture() {};

		private:
			std::vector<GLshort> m_indices;
			GLuint m_vao;
			GLuint m_vbo;
			GLuint m_ibo;
			Shader *m_shader;

			float m_spotBrightness;
			float  m_rayleighBrightness;
			float  m_mieBrightness;
			float  m_mieDistribution;
			int  m_stepCount;
			float m_surfaceHeight;
			float m_rayleighStrength;
			float  m_mieStrength;
			float  m_scatterStrength;
			float m_rayleighCollectionPower;
			float m_mieCollectionPower;
			float m_intensityRed;
			float  m_intensityGreen;
			float m_intensityBlue;
			float m_HExtinctionBias;
			float  m_EyeExtinctionBias;
			float  m_rayleighFactor;
		};
	}; // namespace ecs;
} // namespace omen

#endif //OMEN_SKY_H
