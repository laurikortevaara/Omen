//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
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

			virtual void render(Shader* shader = nullptr);

		protected:
			virtual void initializeShader() {};
			virtual void initializeTexture() {};

		private:
			std::vector<GLshort> m_indices;
			GLuint m_vao;
			GLuint m_vbo;
			GLuint m_ibo;
			Shader *m_shader;
		};
	}; // namespace ecs;
} // namespace omen

#endif //OMEN_SKY_H
