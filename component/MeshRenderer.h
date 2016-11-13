#pragma once

#include "Renderer.h"

class Sprite;
class Texture;

namespace omen {
	class Material;
	namespace ecs {

		class MeshRenderer : public Renderer {
		protected:
		protected:
			virtual void onAttach(ecs::Entity* e);
			virtual void onDetach(ecs::Entity* e);
		public:
			MeshRenderer();
			
			MeshRenderer& setMaterial(const Material& material) {
				m_material = std::make_unique<Material>(material);
				return *this;
			}

			virtual void render();
			
		protected:
		private:
			std::unique_ptr<Material> m_material;
			Texture* m_texture;
			Sprite * m_sprite;
		};

	}
}