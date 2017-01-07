#pragma once

#include "Renderer.h"

namespace omen {
	class Material;
	class Texture;
	class Sprite;
	namespace ui { class Slider; }
	
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
			
			void setShininess(float value) { m_shininess = value; }
		protected:
			void connectSlider(ui::Slider* slider);
		private:
			std::unique_ptr<Material> m_material;
			float m_shininess;
			float m_specularCoeff;
			Texture* m_texture;
			Texture* m_textureNormal;
			Sprite * m_sprite;
			glm::vec3 m_lightDir;
			bool	m_renderNormals;
		};

	}
}