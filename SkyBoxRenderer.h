//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#ifndef OMEN_SKYBOX_RENDERER_H
#define OMEN_SKYBOX_RENDERER_H

#include "Omen_OpenGL.h"
#include <vector>
#include "Shader.h"
#include "component/MeshRenderer.h"

namespace omen {
	namespace ecs {
		class MeshController;

		class SkyBoxRenderer : public MeshRenderer {
		public:
			static Texture * uvTexMap;
			SkyBoxRenderer(MeshController*);
			virtual ~SkyBoxRenderer();

			virtual void render(Shader* shader = nullptr);

		protected:
			virtual void initializeShader() {};
			virtual void initializeTexture() {};

		private:
			Shader *m_shader;
		};
	}; // namespace ecs;
} // namespace omen

#endif //OMEN_SKYBOX_H
