#pragma once

#include "Renderer.h"

namespace omen {
	namespace ecs {
		class MeshRenderer : public Renderer {
		protected:
			virtual void onAttach(std::unique_ptr<ecs::Entity> e);
			virtual void onDetach(std::unique_ptr<ecs::Entity> e);
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
		};

	}
}